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
function islandora_doi_framework_mint_doi($pid, $form, $form_state) {
  $dois = module_invoke_all('islandora_doi_framework_mint', $pid, $form, $form_state);
  if (count($dois)) {
    // We want the one minted in the last hook to fire.
    $doi = array_pop($dois);
  }
  else {
    return FALSE;
  }
  if (preg_match('#^10\..+/.+#', $doi)) {
    return $doi;
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
function islandora_doi_framework_persist_doi($doi, $pid, $form, $form_state) {
  $persists = module_invoke_all('islandora_doi_framework_persist', $doi, $pid, $form, $form_state);
  if (count($persists)) {
    // We want the one persited in the last hook to fire.
    return array_pop($persists);
  }
  else {
    return FALSE;
  }
}

/**
 * Defines hook_islandora_doi_framework_update().
 *
 * @param string $pid
 *   The object's PID.
 * @param string $doi
 *   The object's DOI.
 *
 * @return boolan
 *   TRUE if the DOI (and/or metadata) was updated, FALSE if not.
 */
function islandora_doi_framework_update($pid, $doi) {
  $updates = module_invoke_all('islandora_doi_framework_update', $pid, $doi);
  if (count($updates)) {
    // We want the one updated in the last hook to fire.
    return array_pop($updates);
  }
  else {
    return FALSE;
  }
}

/**
 * Returns a list of PIDs from a file.
 *
 * @todo: Add DOIs and action for update functionality.
 *
 * @param string $path
 *   The absolute path to the file.
 *
 * @return array
 *   A list of PIDs.
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
