<?php

/**
 * @file
 * Drush file for the Islandora DOI module.
 */

/**
 * Implements hook_drush_command().
 */
function islandora_doi_datacite_drush_command() {
  $items = array();
  $items['islandora_doi_datacite_assign_dois_preflight'] = array(
    'aliases' => array('iddadp'),
    'description' => t('Checks objects to make sure they have the required DC metadat to have DOIs assigned to them.'),
    'examples' => array(
      'drush islandora_doi_datacite_assign_dois_preflight --user=admin --pid_file=/tmp/pids.txt',
    ),
    'options' => array(
      'pid_file' => array(
        'description' => t('Absolute path to a file containing PIDs of objects to assign DOIs to, one per line.'),
        'required' => TRUE,
      ),
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  $items['islandora_doi_datacite_assign_dois'] = array(
    'aliases' => array('iddad'),
    'description' => t('Assign DOIs to a list of objects.'),
    'examples' => array(
      'drush islandora_doi_datacite_assign_dois --user=admin --pid_file=/tmp/pids.txt',
    ),
    'options' => array(
      'pid_file' => array(
        'description' => t('Absolute path to a file containing PIDs of objects to assign DOIs to, one per line.'),
        'required' => TRUE,
      ),
      'resource_type' => array(
        'description' => t("The DataCite resource type to use. Must be on of .'Audiovisual', 'Collection', 'Dataset', 'Event', 'Image', 'InteractiveResource', 'Model', 'PhysicalObject', 'Service', 'Software', 'Sound', 'Text', 'Workflow', and 'Other'"),
        'required' => TRUE,
      ),
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  return $items;
}

/**
 * Assigns DOIs to objects.
 */
function drush_islandora_doi_datacite_assign_dois_preflight() {
  module_load_include('inc', 'islandora_doi_framework', 'includes/utilities');
  module_load_include('inc', 'islandora_doi_datacite', 'includes/utilities');

  // Check to see if there is an input file and if there is, use the PIDs
  // from it.
  if (drush_get_option('pid_file')) {
    $input_file_path = drush_get_option('pid_file');
    $pids = islandora_doi_framework_read_input_file($input_file_path);
  }

  // Cycle through the objects and check perform checks on its DC.
  // Write out a log with the PIDs that pass and the ones that fail,
  // explaining the failures.
  foreach ($pids as $pid) {
    $object = islandora_object_load($pid);
    $dc_values = islandora_doi_datacite_get_dc_values($object['DC']->content);
    $errors = islandora_doi_datacite_check_required_values($dc_values);
    $error_file_path = $input_file_path . '.errors';
    $passed_file_path = $input_file_path . '.passed';
    if (count($errors)) {
      drush_log(dt("!pid does not meet DataCite's metadata requirements", array('!pid' => $pid)), 'warning');
      foreach ($errors as $error) {
        file_put_contents($error_file_path, $pid . "\t" . $error[0] . "\t" . $error[1] . "\n", FILE_APPEND);
      }
    }
    else {
      drush_log(dt("!pid meets DataCite's metadata requirements", array('!pid' => $pid)), 'ok');
      file_put_contents($passed_file_path, $pid . "\n", FILE_APPEND);
    }
  }
  if (isset($errors) && count($errors)) {
    drush_print(dt("Please see !error_file_path for details.", array('!error_file_path' => $error_file_path)));
  }
}

/**
 * @todo: when this is working, move to utilities.inc.
 * Checks for required values in the object's DC datastream.
 *
 * @param array $dc_values
 *   An associative array containing the object's DC values.
 *
 * @return array
 *   An array of arrays, each of which has as its first element
 *   a DC element name and as its second element an error message.
 */
function islandora_doi_datacite_check_required_values($dc_values) {
  // We don't include type because the user supplies it on the command line.
  $required_elements = array('title', 'creator', 'publisher', 'date');
  $ret = array();
  foreach ($required_elements as $element) {
    if (!array_key_exists($element, $dc_values)) {
      $ret[] = array(
        $element,
        t('missing'),
      );
    }
    if (array_key_exists($element, $dc_values) && strlen($dc_values[$element][0]) == 0) {
      $ret[] = array(
        $element,
        t('empty'),
      );
    }
  }
  // Parse out a date to see if we can get a yyyy value from it.
  if (!preg_match('/(\d\d\d\d)/', $dc_values['date'][0], $matches)) {
    $ret[] = array('date', t('does not have a YYYY year'));
  }
  return $ret;
}

/**
 * Assigns DOIs to objects.
 */
function drush_islandora_doi_datacite_assign_dois() {
  module_load_include('inc', 'islandora_doi_framework', 'includes/utilities');

  if (!drush_confirm(dt("You are about to mint new DOIs. Have you run the preflight check?",
    array()))) {
    drush_user_abort(dt('Exiting, please run the preflight check.'));
    exit;
  }

  // Check to see if there is an input file and if there is, use the PIDs
  // from it.
  if (drush_get_option('pid_file')) {
    $input_file_path = drush_get_option('pid_file');
    $pids = islandora_doi_framework_read_input_file($input_file_path);
  }

  // @todo: Validate resource type value.
  $resource_type = drush_get_option('resource_type', '');

  // Cycle through the objects and assign the DOIs.
  foreach ($pids as $pid) {
    $object = islandora_object_load($pid);
    $dc_values = islandora_doi_datacite_get_dc_values($object['DC']->content);
    // In the GUI version, we get required metadata values from the form state
    // of islandora_doi_framework_manage_doi. In the drush version, there is no
    // form, so we need to populate this array with the necessary values from
    // the DC datastream:
    $form_state = array();
    preg_match('/(\d\d\d\d)/', $dc_values['date'], $matches);
    $year = trim($matches[1]);
    $form_state['values'] = array(
      'islandora_doi_datacite_title' => $dc_values['title'],
      'islandora_doi_datacite_creator' => $dc_values['creator'],
      'islandora_doi_datacite_date' => $year,
      'islandora_doi_datacite_publisher' => $dc_values['publisher'],
      'islandora_doi_datacite_resourcetype' => $resource_type,
    );
    if ($doi = islandora_doi_datacite_post_new_via_drush($pid, array(), $form_state)) {
      if (islandora_doi_framework_persist_doi($doi, $pid)) {
        drush_log(dt('DOI !doi assigned to object !pid',
          array('!doi' => $doi, '!pid' => $pid)), 'ok');
      }
    }
  }
}

/**
 * Implements drush_COMMAND_validate().
 */
function drush_islandora_doi_datacite_assign_dois_validate() {
  $resource_types = array('Audiovisual',
    'Collection',
    'Dataset',
    'Event',
    'Image',
    'InteractiveResource',
    'Model',
    'PhysicalObject',
    'Service',
    'Software',
    'Sound',
    'Text',
    'Workflow',
    'Other',
  );
  $type = drush_get_option('resource_type');
  if (!in_array($type, $resource_types)) {
    drush_set_error('INVALID DATACITE RESOURCE TYPE', $type);
  }
}

/**
 * @todo: when this is working, move to utilities.inc.
 *
 * Registers a DOI and associated metadata with DataCite.
 *
 * Also checks to make sure that the source DC datastream
 * contains the elements required by the DataCite metadata
 * schema.
 *
 * The resource's metadata must be registered via the DataCite MDS API
 * first, then its URL. See https://datacite.readme.io/docs/mds-2 for
 * additional info.
 *
 * @param string $pid
 *   The PID of the object.
 * @param array $form_state
 *   An array containing values in the same structure as the
 *   islandora_doi_framework_manage_doi form.
 *
 * @return string|bool
 *   The DOI if the request was successful, FALSE if not.
 */
function islandora_doi_datacite_post_new_via_drush($pid, $form_state) {
  $symbol = variable_get('islandora_doi_datacite_username', 'CISTI.FOO');
  $password = variable_get('islandora_doi_datacite_password', '');
  $suffix_source = variable_get('islandora_doi_datacite_suffix_source', 'pid');
  if ($suffix_source == 'pid') {
    $doi = variable_get('islandora_doi_datacite_prefix', '10.99999/') . $pid;
    $doi = trim($doi);
  }
  if ($suffix_source == 'uuid') {
    $uuid = islandora_doi_datacite_get_uuid();
    $doi = variable_get('islandora_doi_datacite_prefix', '10.99999/') . $uuid;
    $doi = trim($doi);
  }

  // POST the metadata first. This is a requirement of the API.
  // The DOI for the resource must be encoded in the XML in the
  // <identifier identifierType="DOI"> element.
  $api_url = variable_get('islandora_doi_datacite_api_url', 'https://mds.datacite.org/') . 'metadata';

  $metadata_xml = islandora_doi_datacite_generate_metadata($pid, $doi, $form_state);

  $response = drupal_http_request($api_url, array(
    'headers' => array(
      'Content-Type' => 'application/xml;charset=UTF-8',
      'Authorization' => 'Basic ' . base64_encode($symbol . ':' . $password),
    ),
    'method' => 'POST',
    'data' => $metadata_xml,
  ));
  if ($response->code != 201) {
    if ($response->code == '400') {
      // The DataCite API provides specific error messages in the response body.
      $message = $response->status_message . ': ' . $response->data;
    }
    else {
      $message = $response->status_message;
    }
    watchdog(
      'islandora_doi_datacite',
      'Error registering metadata for object !pid, so cannot mint DOI: !code, !message',
      array(
        '!pid' => $pid,
        '!code' => $response->code,
        '!message' => $message,
      ),
      WATCHDOG_ERROR
    );
    drupal_set_message(t("Can't post DataCite metadata for object !pid. See the Drupal log for more information.", array('!pid' => $pid)), 'error');
    return FALSE;
  }

  // Then POST the resource's URL to mint the DOI.
  $api_url = variable_get('islandora_doi_datacite_api_url', 'https://mds.datacite.org/') . 'doi';
  global $base_url;
  $url = $base_url . '/islandora/object/' . $pid;
  $data = array('doi' => $doi, 'url' => $url);
  $response = drupal_http_request($api_url, array(
    'headers' => array(
      'Content-Type' => 'text/plain;charset=UTF-8',
      'Authorization' => 'Basic ' . base64_encode($symbol . ':' . $password),
    ),
    'method' => 'POST',
    'data' => 'doi=' . $doi . PHP_EOL . 'url=' . $url,
  ));
  if ($response->code == 201) {
    drupal_set_message(t('DOI !doi successfully minted for object !pid', array('!doi' => $doi, '!pid' => $pid)));
    return $doi;
  }
  else {
    if ($response->code == '400') {
      // The DataCite API provides specific error messages in the response body.
      $message = $response->status_message . ': ' . $response->data;
    }
    else {
      $message = $response->status_message;
    }
    watchdog(
      'islandora_doi_datacite',
      'Error minting DOI !doi for object !pid: !code, !message',
      array(
        '!doi' => $doi,
        '!pid' => $pid,
        '!code' => $response->code,
        '!message' => $message),
      WATCHDOG_ERROR
    );
    drupal_set_message(t("Can't mint DOI for object !pid. See the Drupal log for more information.", array('!pid' => $pid)), 'error');
    return FALSE;
  }
}
