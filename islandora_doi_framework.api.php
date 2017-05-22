<?php
/**
 * @file
 * This file documents all available hook functions provided by the
 * Islandora DOI Framework module.
 *
 * Note that the Islandora DOI Framework module does not manage any
 * admin settings. Submodules will need to provide and manage whatever
 * configuration settings they need for minting and persisting DOIs,
 * such as API endpoint URLs, API keys, etc.
 */

/**
 * Mints a DOI using an external API.
 *
 * Note that if multiple implementations of this hook exist,
 * the value from the last implementation is used. Implementations
 * are responsible for issuing feedback to the user and for
 * logging success/failure.
 *
 * @param string $pid
 *   The object's PID.
 *
 * @return string|bool
 *   The DOI that was minted, false if DOI was not minted.
 */
function hook_islandora_doi_framework_mint($pid) {
  // Each institution has its own DOI prefix (the part to the left of the /),
  // assigned by an registration agency
  // (http://www.doi.org/registration_agencies.html). The suffix (the part to
  // the right of the /) is assigned by the organization that
  // wishes to register DOI names (publisher, university, etc.).
  drupal_set_message(t("DOI !doi assigned to object !pid.", array('!pid' => $pid, '!doi' => $doi)));
  $doi = '10.99999/' . $pid;
  return $doi;

}

/**
 * Saves an object's DOI somewhere (e.g., in a datastream).
 *
 * Note that if multiple implementations of this hook exist,
 * the value from the last implementation is used. Implementations
 * are responsible for issuing feedback to the user and for
 * logging success/failure.
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
  // Add the DOI to MODS, etc., then return a boolean value.
  drupal_set_message(t("DOI !doi saved for object !pid.", array('!pid' => $pid, '!doi' => $doi)));
  return TRUE;
}

/**
 * Updates a DOI.
 *
 * Performs updates to the object's DOI in the registrar (i.e., where it was
 * minted), i.e., updates the DOI's URL, the metadata associated with it, or
 *  both. Implementations are responsible for issuing feedback to the user
 * and for logging success/failure.
 *
 * @param string $pid
 *   The object's PID.
 * @param string $doi
 *   The object's DOI (either a DOI name or a resolvable URL).
 * @param string $action
 *   One of 'url', 'metadata', or 'both'
 *
 * @return bool
 *   TRUE if the DOI was updated, FALSE if not.
 */
function hook_islandora_doi_framework_update($pid, $doi, $action = 'both') {
  // Update the object's metadata at the registrar, or update the object's
  // URL at the registrar, then return a boolean value.
  return TRUE;
}

/**
 * Checks locally for whether an object has a DOI, e.g. in a MODS datastream.
 *
 * @param string $pid
 *   The object's PID.
 *
 * @return string|bool
 *   The value of the DOI if present, FALSE otherwise.
 */
function hook_islandora_doi_framework_check_for_doi($pid) {
  // Check the object's MODS datastream, etc. for a presence of a DOI.
  return FALSE;
}
