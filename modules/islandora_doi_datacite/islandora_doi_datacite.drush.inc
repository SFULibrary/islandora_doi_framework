<?php

/**
 * @file
 * Drush file for the Islandora DOI module.
 *@todo: Add a command to check to see if all the objects have the required DC elements.
 */

/**
 * Implements hook_drush_command().
 */
function islandora_doi_datacite_drush_command() {
  $items = array();
  $items['islandora_doi_datacite_assign_dois_preflight'] = array(
    'aliases' => array('iddadp'),
    'description' => 'Checks objects to make sure they have the required DC metadat to have DOIs assigned to them.',
    'examples' => array(
      'drush islandora_doi_datacite_assign_dois_preflight --user=admin --pid_file=/tmp/pids.txt',
    ),
    'options' => array(
      'pid_file' => array(
        'description' => 'Absolute path to a file containing PIDs of objects to assign DOIs to, one per line.',
      ),
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  $items['islandora_doi_datacite_assign_dois'] = array(
    'aliases' => array('iddad'),
    'description' => 'Assign DOIs to a list of objects.',
    'examples' => array(
      'drush islandora_doi_datacite_assign_dois --user=admin --pid_file=/tmp/pids.txt',
    ),
    'options' => array(
      'pid_file' => array(
        'description' => 'Absolute path to a file containing PIDs of objects to assign DOIs to, one per line.',
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
    // @todo: Check to see if the object's DC has all the right stuff.
  }
}

/**
 * Assigns DOIs to objects.
 */
function drush_islandora_doi_datacite_assign_dois() {
  module_load_include('inc', 'islandora_doi_framework', 'includes/utilities');

  // Check to see if there is an input file and if there is, use the PIDs
  // from it.
  if (drush_get_option('pid_file')) {
    $input_file_path = drush_get_option('pid_file');
    $pids = islandora_doi_framework_read_input_file($input_file_path);
  }

  // Cycle through the objects and assign the DOIs.
  foreach ($pids as $pid) {
    if ($doi = islandora_doi_framework_mint_doi($pid)) {
      if (islandora_doi_framework_persist_doi($doi, $pid)) {
        drush_log(dt('DOI !doi assigned to object !pid',
          array('!doi' => $doi, '!pid' => $pid)), 'ok');
      }
    }
  }
}


/**
 * Registers a DOI and associated metadata with DataCite.
 *
 * Note: this is based on the islandora_doi_datacite_post_new() function
 * used in the GUI.
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
 * @param array $form
 *   The islandora_doi_framework_manage_doi form.
 *
 *
 * @return string|bool
 *   The DOI if the request was successful, FALSE if not.
 */
function islandora_doi_datacite_post_new_via_drush($pid, $form, $form_state) {
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

  // @todo: In the GUI version, the form state of islandora_doi_framework_manage_doi form on submission.
  // In the drush version, there is no form, so we need to populate this array with the
  // necessary values from the DC datastream.
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
