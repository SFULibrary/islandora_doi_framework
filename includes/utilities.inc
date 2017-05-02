<?php

/**
 * @file
 * Utilities.
 */

/**
 * Defines hook_islandora_doi_framework_mint().
 *
 * @param string $pid
 *   The object's PID.
 *
 * @return string|bool
 *   The DOI that was fetched, FALSE if DOI was not fetched.
 */
function islandora_doi_framework_mint_doi($pid) {
  $dois = module_invoke_all('islandora_doi_framework_mint', $pid);
  if (count($dois)) {
    return $dois[0];
  }
  else {
    return FALSE;
  }
}

/**
 * Defines hook_islandora_doi_framework_persist().
 *
 * @param string $doi
 *   The fetched DOI.
 * @param string $pid
 *   The object's PID.
 *
 * @return boolan
 *   True if the DOI was saved, false if not.
 */
function islandora_doi_framework_persist_doi($doi, $pid) {
  $dois_saved = module_invoke_all('islandora_doi_framework_persist', $doi, $pid);
  // We only want the first member, and we want to be able to test
  // whether it is the value TRUE.
  if (count($dois_saved) > 1) {
    $dois_saved = array_slice($dois_saved, 0, 1, TRUE);
  }

  if (in_array(TRUE, $dois_saved)) {
    watchdog('islandora_doi_framework', 'DOI !doi assigned successfully to object !pid',
      array('!doi' => $doi, '!pid' => $pid));
    return $dois_saved[0];
  }
  else {
    watchdog('islandora_doi_framework', 'DOI !doi was not assigned to object !pid',
      array('!doi' => $doi, '!pid' => $pid), WATCHDOG_ERROR);
    return FALSE;
  }
}

/**
 * Returns a list of PIDs from a PID file.
 *
 * @param string $pid_file_path
 *   The absolute path to the PID file.
 *
 * @return array
 *   A list of PIDs.
 */
function islandora_doi_framework_read_pid_file($pid_file_path) {
  if (!file_exists($pid_file_path)) {
    drush_set_error('PID_FILE_NOT_FOUND',
    dt('The specified PID file (!pid_file) does not exist.',
      array('!pid_file' => $pid_file_path)));
    drupal_exit();
  }

  $pids = array();
  $lines = file($pid_file_path);
  foreach ($lines as $pid) {
    $pid = trim($pid);
    // Skip commented out rows.
    if (!preg_match('!(#|//)!', $pid)) {
      $pids[] = $pid;
    }
  }

  if (!count($pids) || !$pids) {
    drush_set_error('PID_FILE_IS_EMPTY',
    dt('The specified PID file (!pid_file) is empty.',
      array('!pid_file' => $pid_file_path)));
    drupal_exit();
  }

  return $pids;
}
