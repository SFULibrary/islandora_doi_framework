<?php

/**
 * @file
 * Utilities.
 */

/**
 * @return string|boolean
 *    The DOI that was fetched, false if DOI was not fetched.
 */
function islandora_doi_fetch_doi() {
  $dois = module_invoke_all('islandora_doi_fetch');
  return $dois[0];
}

/**
 * @param string $doi
 *    The fetched DOI.
 * @param string $pid
 *    The object's PID.
 *
 * @return boolan
 *    True if the DOI was saved, false if not.
 */
function islandora_doi_save_doi($doi, $pid) {
  $dois_saved = module_invoke_all('islandora_doi_save', $doi, $pid);
  if (count($dois_saved)) {
    return $dois_saved[0];
  }
  else {
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
function islandora_doi_read_pid_file($pid_file_path) {
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
