<?php

/**
 * @file
 * This file contains all theme and preprocess functions.
 */

/**
 * Prepares variables for use in datacite-metadata.tpl.php.
 */
function template_preprocess_datacite_metadata(&$variables) {
  // title, publication_year, publisher, and resource_type are all required
  // in the DOI minting form so we don't preprocess them here.
  $variables['creators'] = explode(';', $variables['creators']);
  $variables['subjects'] = isset($variables['dc_values']['subject']) ? $variables['dc_values']['subject'] : array();
  $variables['languages'] = isset($variables['dc_values']['language']) ? $variables['dc_values']['language'] : array();
  $variables['descriptions'] = isset($variables['dc_values']['description']) ? $variables['dc_values']['description'] : array();
}
