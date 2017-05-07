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
  if (count($dois) && strlen($dois[0])) {
    return array_pop($dois);
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
 */
function islandora_doi_framework_persist_doi($doi, $pid) {
  module_invoke_all('islandora_doi_framework_persist', $doi, $pid);
}

/**
 * Defines hook_islandora_doi_framework_update().
 *
 * @param string $doi
 *   The fetched DOI.
 * @param string $pid
 *   The object's PID.
 * @param string $action
 *   One of 'url', 'metadata', or 'both'
 *
 * @return boolan
 *   True if the DOI was updated, false if not.
 */
function islandora_doi_framework_update_doi($doi, $pid, $action = 'both') {
  $dois_updated = module_invoke_all('islandora_doi_framework_update', $doi, $pid, $action);
  // @todo: the rest of this function....
}

/**
 * Returns a list of PIDs or DOIs from a file.
 *
 * @param string $path
 *   The absolute path to the file.
 *
 * @return array
 *   A list of PIDs or DOIs.
 */
function islandora_doi_framework_read_input_file($path) {
  if (!file_exists($path)) {
    drush_set_error('INPUT_FILE_NOT_FOUND',
    dt('The specified input file (!file) does not exist.',
      array('!file' => $path)));
    drupal_exit();
  }

  $items = array();
  $lines = file($path);
  foreach ($lines as $item) {
    $item = trim($item);
    // Skip commented out rows.
    if (!preg_match('!(#|//)!', $item)) {
      $items[] = $item;
    }
  }

  if (!count($items) || !$items) {
    drush_set_error('INPUT_FILE_IS_EMPTY',
    dt('The specified input file (!file) is empty.',
      array('!file' => $path)));
    drupal_exit();
  }

  return $items;
}
