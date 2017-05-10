<?php

/**
 * @file
 * This file contains all theme and preprocess functions.
 */

/**
 * Prepares variables for use in datacite-metadata.tpl.php.
 */
function template_preprocess_datacite_metadata(&$variables) {
  // Populate variables with selected DC values. DC.date and DC.type have
  // already been validated to conform to DataCite metadata requirements
  // and are not reprocessed here.
  $variables['creators'] = $variables['dc']['creator'];
  $variables['titles'] = isset($variables['dc']['title']) ? $variables['dc']['title'] : array();
  $variables['publishers'] = isset($variables['dc']['publisher']) ? $variables['dc']['publisher'] : array();
  $variables['subjects'] = isset($variables['dc']['subject']) ? $variables['dc']['subject'] : array();
  $variables['languages'] = isset($variables['dc']['language']) ? $variables['dc']['language'] : array();
  $variables['descriptions'] = isset($variables['dc']['description']) ? $variables['dc']['description'] : array();
}
