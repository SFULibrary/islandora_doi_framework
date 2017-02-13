<?php

/**
 * @file
 * Drush file for the Islandora DOI module.
 */

/**
 * Implements hook_drush_command().
 */
function islandora_doi_drush_command() {
  $items = array();
  $items['islandora_doi_assign_dois'] = array(
    'aliases' => array('idad'),
    'description' => 'Assign DOIs to a list of objects.',
    'examples' => array(
      'drush islandora_doi_assign_dois --user=admin --pid_file=/tmp/pids.txt',
    ),
    'options' => array(
      'pid_file' => array(
        'description' => 'Absolute path to a file containing PIDs of objects to assign DOIs to, per line.',
      ),
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  return $items;
}

/**
 * Purges objects (including page objects).
 */
function drush_islandora_doi_assign_dois() {
  module_load_include('inc', 'islandora_doi', 'includes/utilities');

  // Check to see if there is a PID file and if there is, use the PIDs
  // from it.
  if (drush_get_option('pid_file')) {
    $pid_file_path = drush_get_option('pid_file');
    $pids = islandora_doi_read_pid_file($pid_file_path);
  }

  // Cycle through the objects and assign the DOIs.
  foreach ($pids as $pid) {
    if ($doi = islandora_doi_fetch_doi()) {
      if (islandora_doi_save_doi($doi, $pid)) {
        drush_log(dt('DOI !doi assigned successfully to object !pid',
          array('!doi' => $doi, '!pid' => $pid)), 'ok');
      }
      else {
        drupal_set_error('DOI_NOT_ASSIGNED', dt('DOI !doi was assigned to object !pid',
          array('!doi' => $doi, '!pid' => $pid)));
      }
    }
  }
}
