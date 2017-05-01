<?php

/**
 * @file
 * Utility functions.
 */


/**
 * Parses the DC datastream into an array.
 *
 * @param string $xml
 *   The XML content of the DC datastream.
 *
 * @return array
 *   An associative array containing element name => element values.
 */
function islandora_doi_datacite_mods_get_dc_values($xml) {
  $dc_values = array();
  $dom = new DOMDocument;
  $dom->loadXML($xml);
  $elements = $dom->getElementsByTagNameNS('http://purl.org/dc/elements/1.1/', '*');
  foreach ($elements as $e) {
    if (!array_key_exists($e->localName, $dc_values)) {
      $dc_values[$e->localName] = array();
      $dc_values[$e->localName][] = $e->nodeValue;
    }
    else {
      $dc_values[$e->localName][] = $e->nodeValue;
    }
  }
  return $dc_values;
}
