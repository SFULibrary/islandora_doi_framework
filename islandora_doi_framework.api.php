<?php
/**
 * @file
 * This file documents all available hook functions provided by the
 * Islandora DOI Framework module.
 */

/**
 * Fetches a DOI from an external API.
 *
 * Note that if multiple implementations of this hook exist,
 * the value from the first implementation is used.
 *
 * @return string|bool
 *   The DOI that was fetched, false if DOI was not fetched.
 */
function hook_islandora_doi_framework_mint() {
  // Go get the DOI from an API, etc.
  return 'doi:999921391';
}

/**
 * Saves a DOI somewhere (e.g., in a datastream).
 *
 * Note that if multiple implementations of this hook exist,
 * the value from the first implementation is used.
 *
 * @param string $doi
 *   The fetched DOI.
 * @param string $pid
 *   The object's PID.
 *
 * @return bool
 *   True if the DOI was saved, false if not.
 */
function hook_islandora_doi_framework_persist($doi, $pid) {
  // Add the DOI to MODS, etc.
  return TRUE;
}

/**
 * Updates a DOI.
 *
 * Performs updates to the DOI in the registrar (i.e., where it
 * was minted) and locally (i.e., where it was peristed).
 *
 * @param string $doi
 *   The fetched DOI.
 * @param string $pid
 *   The object's PID.
 *
 * @return bool
 *   True if the DOI was updated, false if not.
 */
function hook_islandora_doi_framework_update($doi, $pid) {
  // Update the object's URL locally, and/or metadata at the
  // registrar, if necessary.
  return TRUE;
}
