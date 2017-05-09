<?php

/**
 * @file
 * Drush file for the Islandora DOI module.
 */

/**
 * Implements hook_drush_command().
 */
function islandora_doi_framework_drush_command() {
  $items = array();
  $items['islandora_doi_framework_assign_dois'] = array(
    'aliases' => array('idfad'),
    'description' => 'Assign DOIs to a list of objects.',
    'examples' => array(
      'drush islandora_doi_frameork_assign_dois --user=admin --objects=/tmp/pids.txt',
    ),
    'options' => array(
      'objects' => array(
        'description' => 'Absolute path to a file containing PIDs of objects to assign DOIs to, one per line.',
      ),
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  $items['islandora_doi_framework_update_dois'] = array(
    'aliases' => array('idfud'),
    'description' => 'Update the DOIs of a list of objects.',
    'examples' => array(
      'drush islandora_doi_frameork_update_dois --user=admin --objects=/tmp/dois.txt',
    ),
    'options' => array(
      'objects' => array(
        'description' => 'Absolute path to a file containing either PIDs of objects whose DOIs are ' .
        'to be updated, or contianing DOIs to update, plus the corresponding PIDs and an action, ' .
        'which is one of "url", "metadata", or "both". One tab-separated entry per line.',
      ),
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  return $items;
}

/**
 * Assigns DOIs to objects.
 */
function drush_islandora_doi_framework_assign_dois() {
  module_load_include('inc', 'islandora_doi_framework', 'includes/utilities');

  // Check to see if there is an input file and if there is, use the PIDs
  // from it.
  if (drush_get_option('objects')) {
    $input_file_path = drush_get_option('objects');
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
 * Updates DOIs.
 *
 * Note: this functionality is not yet complete.
 */
function drush_islandora_doi_framework_update_dois() {
  module_load_include('inc', 'islandora_doi_framework', 'includes/utilities');

  // Check to see if there is an input file and if there is, use the data
  // from it.
  if (drush_get_option('objects')) {
    $input_file_path = drush_get_option('objects');
    $rows = islandora_doi_framework_read_input_file($input_file_path);
  }

  // Cycle through the objects and update the DOIs.
  foreach ($rows as $row) {
    list($pid, $doi, $action) = explode("\t", $row);
    if ($updated = islandora_doi_framework_update_doi($pid, $doi, $action)) {
      if (islandora_doi_framework_persist_doi($doi, $pid)) {
        drush_log(dt('DOI !doi assigned to object !pid',
          array('!doi' => $doi, '!pid' => $pid)), 'ok');
      }
    }
  }
}
