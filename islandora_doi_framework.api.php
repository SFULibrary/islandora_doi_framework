<?php
/**
 * @file
 * This file documents all available hook functions provided by the
 * Islandora DOI Framework module.
 *
 * See the sample mint and persist modules for further examples.
 *
 * Note that the Islandora DOI Framework module does not manage any
 * admin settings. Submodules will need to provide and manage whatever
 * configuration settings they need for minting and persisting DOIs,
 * such as API endpoint URLs, API keys, etc.
 */

/**
 * Registers handlers for the islandora_doi_framework_manage_doi form.
 *
 * @return array
 *   An array with two keys, 'assign_doi' and 'update_doi', each of which has
 *   two members, 'validate' and 'submit'. Each of these has as their
 *   value an array of validate and submit functions. The 'assign_doi',
 *   'update_doi', and their respective 'validate' and 'submit' keys should
 *   all exist, and the 'validate' and 'submit' keys should return empty
 *   arrays if necessary.
 *
 * Note that hook_islandora_doi_framework_mint() and
 * hook_islandora_doi_framework_persist() are fired even if this hook is not
 * implemented. Therefore, this hook is optional. Implement it if you want to
 * perform additional tasks on submission of the
 * islandora_doi_framework_manage_doi form, like a custom
 * drupal_set_message(), or if you need to perform some custom validation.
 *
 * A common use for registering a validate handler is if you are altering
 * the islandora_doi_framework_manage_doi form and you want to validate
 * the value supplied in your custom form element.
 */
function hook_islandora_doi_framework_form_handlers() {
  return array(
    'assign_doi' => array(
      'validate' => array('my_assign_validate_function'),
      'submit' => array('my_first_assign_submit_function',
        'my_second_assign_submit_function'),
    ),
    'update_doi' => array(
      'validate' => array(),
      'submit' => array('my_update_submit_function'),
    ),
  );
}

/**
 * Mints a DOI using an external API.
 *
 * See the islandora_doi_framework_sample_mint module for an example.
 *
 * Note that if multiple implementations of this hook exist,
 * the value from the last implementation is used. Implementations
 * are responsible for issuing feedback to the user and for
 * logging success/failure.
 *
 * @param string $pid
 *   The object's PID.
 * @param array $form
 *   The islandora_doi_framework_manage_doi form.
 * @param array $form_state
 *   The form state of islandora_doi_framework_manage_doi form on submission.
 *
 * @return string|bool
 *   The DOI that was minted, FALSE if DOI was not minted.
 */
function hook_islandora_doi_framework_mint($pid, $form, $form_state) {
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
 * See the islandora_doi_framework_sample_perist module for an example.
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
 * @param array $form
 *   The islandora_doi_framework_manage_doi form.
 * @param array $form_state
 *   The form state of islandora_doi_framework_manage_doi form on submission.
 *
 * @return bool
 *   TRUE if the DOI was saved, FALSE if not.
 */
function hook_islandora_doi_framework_persist($doi, $pid, $form, $form_state) {
  // Add the DOI to MODS, etc., then return a boolean value.
  drupal_set_message(t("DOI !doi saved for object !pid.", array('!pid' => $pid, '!doi' => $doi)));
  return TRUE;
}

/**
 * Updates a DOI.
 *
 * Performs updates to the object's DOI in the registrar (i.e., where it was
 * minted), i.e., updates the DOI's URL and the metadata associated with it.
 * Implementations are responsible for issuing feedback to the user and for
 * logging success/failure.
 *
 * @param string $pid
 *   The object's PID.
 * @param string $doi
 *   The object's DOI.
 * @param array $form
 *   The islandora_doi_framework_manage_doi form.
 * @param array $form_state
 *   The form state of islandora_doi_framework_manage_doi form on submission.
 *
 * @return bool
 *   TRUE if the URL and metadata is now up to date, FALSE if not.
 */
function hook_islandora_doi_framework_update($pid, $doi, $form, $form_state) {
  // Update the object's metadata and URL at the registrar,
  // then return a boolean value.
  return TRUE;
}

/**
 * Checks locally for whether an object already has a DOI, e.g. in a MODS
 * datastream.
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
