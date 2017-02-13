<?php
/**
 * @file
 * This file documents all available hook functions provided by the Islandora DOI module.
 */

/**
 * Fetches a DOI from an external API.
 *
 * Note that if multiple implementations of this hook exist,
 * the value from the first implementation is used.
 *
 * @return string|boolean
 *    The DOI that was fetched, false if DOI was not fetched.
 */
function hook_islandora_doi_fetch() {
  return 'doi:999921391';
}

/**
 * Saves a DOI somewhere (e.g., in a datastream).
 *
 * Note that if multiple implementations of this hook exist,
 * the value from the first implementation is used.
 *
 * @param string $doi
 *    The fetched DOI.
 * @param string $pid
 *    The object's PID.
 *
 * @return boolean
 *    True if the DOI was saved, false if not.
 */
function hook_islandora_doi_save($doi, $pid) {
  return TRUE;
}
