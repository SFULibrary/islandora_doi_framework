<?php
/**
 * @file
 * This file documents all available hook functions provided by the
 * Islandora DOI Datacite module.
 */

/**
 * Alters the Datacite XML produced by this module.
 *
 * @param string $metadata_xml
 *   The serialized Datacite XML string.
 * @param string $pid
 *   The current object's PID.
 * @param array $form_state
 *   The form state of the form that mints a new Datacite DOI.
 */
function hook_islandora_doi_datacite_metadata_xml_alter(&$metadata_xml, &$pid, &$form_state) [
  // Modify the Datacite XML, either by simple preg_replace() on the serialized
  // string, or by using the DOM, etc.

  // You would not typically modify $pid or $form_state, they are passed by
  // reference here to conform to drupal_alter().
} 
