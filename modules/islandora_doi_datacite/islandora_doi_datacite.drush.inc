<?php

/**
 * @file
 * Drush file for the Islandora DOI DataCite module.
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

    // We need to check here whether the object already has a DOI. In a GUI
    // context, hook_islandora_doi_framework_check_for_doi() is defined in
    // the function that prepares the user-facing assignment form, so we
    // can't rely on it here.
    $dois = module_invoke_all('islandora_doi_framework_check_for_doi', $pid);
    $dois = array_unique($dois);
    if (count($dois) && strlen($dois[0])) {
      $dois = array_unique($dois);
      // Put any non-boolean values at beginning of array.
      rsort($dois);
      drush_log(dt("!pid already has a DOI: !doi", array('!pid' => $pid, '!doi' => $dois[0])), 'warning');
      file_put_contents($error_file_path, $pid . "\t" . "DOI already exists\t" . $dois[0] . "\n", FILE_APPEND);
    }

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
 * Assigns DOIs to objects.
 */
function drush_islandora_doi_datacite_assign_dois() {
  module_load_include('inc', 'islandora_doi_framework', 'includes/utilities');
  module_load_include('inc', 'islandora_doi_datacite', 'includes/utilities');

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

  $resource_type = drush_get_option('resource_type', '');

  // Cycle through the objects and assign the DOIs.
  foreach ($pids as $pid) {
    $object = islandora_object_load($pid);

    // We need to check here whether the object already has a DOI. In a GUI
    // context, hook_islandora_doi_framework_check_for_doi() is defined in
    // the function that prepares the user-facing assignment form, so we
    // can't rely on it here.
    $dois = module_invoke_all('islandora_doi_framework_check_for_doi', $pid);
    $dois = array_unique($dois);
    if (count($dois) && strlen($dois[0])) {
      $dois = array_unique($dois);
      // Put any non-boolean values at beginning of array.
      rsort($dois);
      drush_log(dt("!pid already has a DOI: !doi", array('!pid' => $pid, '!doi' => $dois[0])), 'warning');
      continue;
    }

    $dc_values = islandora_doi_datacite_get_dc_values($object['DC']->content);
    $errors = islandora_doi_datacite_check_required_values($dc_values);
    if (count($errors)) {
      drush_log(dt("!pid does not meet DataCite's metadata requirements; skipping", array('!pid' => $pid)), 'warning');
      foreach ($errors as $error) {
        drush_log(dt("Field !field in object !pid: !error",
          array(
            '!field' => $error[0],
            '!pid' => $pid,
            '!error' => $error[1],
          )), 'warning');
      }
      // If there are errors, move on.
      continue;
    }

    // In the GUI version, we get required metadata values from the form state
    // of islandora_doi_framework_manage_doi. In the drush version, there is no
    // form, so we need to populate this array with the necessary values from
    // the DC datastream:
    $form_state = array();
    preg_match('/(\d\d\d\d)/', $dc_values['date'][0], $matches);
    $year = trim($matches[1]);
    $form_state = array();
    $form_state['values'] = array(
      'islandora_doi_datacite_title' => $dc_values['title'][0],
      'islandora_doi_datacite_creator' => $dc_values['creator'][0],
      'islandora_doi_datacite_date' => $year,
      'islandora_doi_datacite_publisher' => $dc_values['publisher'][0],
      'islandora_doi_datacite_resourcetype' => $resource_type,
    );
    if ($doi = islandora_doi_datacite_post_new_via_drush($pid, $form_state)) {
      if (islandora_doi_framework_persist_doi($doi, $pid, array(), $form_state)) {
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
  $resource_types = array(
    'Audiovisual',
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
    drush_set_error('INVALID_DATACITE_RESOURCE_TYPE', dt('--resource_type !type is not valid', array('!type' => $type)));
  }
}
