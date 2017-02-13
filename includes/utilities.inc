<?php

/**
 * @file
 * Utilities.
 */

/**
 * @return string|boolean
 *    The DOI was fetched, false if not.
 */
function islandora_doi_fetch_doi() {
  return 'doi:12345';
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
  dd($doi, "DOI");
  dd($pid, "PID");
  return TRUE;
}
