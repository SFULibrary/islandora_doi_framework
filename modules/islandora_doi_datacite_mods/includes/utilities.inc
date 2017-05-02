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

/**
 * Registers a DOI and associated metadata with DataCite.
 *
 * The resource's metadata must be registered first, then its URL.
 * See https://datacite.readme.io/docs/mds-2 for additional info.
 *
 * @param string $pid
 *   The PID of the object.
 *
 * @return string|bool
 *   The DOI if the request was successful, FALSE if not.
 */
function islandora_doi_datacite_mods_post_new($pid) {
  $symbol = variable_get('islandora_doi_datacite_mods_username', 'CISTI.FOO');
  $password = variable_get('islandora_doi_datacite_mods_password', '');
  $doi = variable_get('islandora_doi_datacite_mods_prefix', '10.99999/') . $pid;

  // POST the metadata first. This is a requirement of the API.
  // The DOI for the resource must be encoded in the XML in the
  // <identifier identifierType="DOI"> element.
  $api_url = variable_get('islandora_doi_datacite_mods_api_url', 'https://mds.datacite.org/') . 'metadata';

  // !Dev only - $metadata_xml should be populated from the object's meatadata.
  $metadata_xml = file_get_contents('/tmp/datacite-example-dataset-v4.0.xml');

  $response = drupal_http_request($api_url, array(
    'headers' => array(
      'Content-Type' => 'application/xml;charset=UTF-8',
      'Authorization' => 'Basic ' . base64_encode($symbol . ':' . $password),
    ),
    'method' => 'POST', 
    'data' => $metadata_xml,
  ));
  if ($response->code != 201) {
    watchdog(
      'islandora_doi_datacite_mods',
      'Error registering metadata for object !pid, so cannot proceed: !code, !message',
      array('!pid' => $pid, '!code' => $response->code, '!message' => $response->status_message),
      WATCHDOG_ERROR
    );
    return FALSE;
  }

  // Then POST the resource's URL.
  $api_url = variable_get('islandora_doi_datacite_mods_api_url', 'https://mds.datacite.org/') . 'doi';
  global $base_url;
  $url = $base_url . '/islandora/object/' . $pid;
  $data = array('doi' => $doi, 'url' => $url);
  $response = drupal_http_request($api_url, array(
    'headers' => array(
      'Content-Type' => 'text/plain;charset=UTF-8',
      'Authorization' => 'Basic ' . base64_encode($symbol . ':' . $password),
    ),
    'method' => 'POST', 
    'data' => 'doi=' . $doi . PHP_EOL . 'url=' . $url,
  ));
  if ($response->code == 201) {
    return $doi;
  }
  else {
    watchdog(
      'islandora_doi_datacite_mods',
      'Error assigning URL to DOI !doi for object !pid: !code, !message',
      array('!doi' => $doi, '!pid' => $pid, '!code' => $response->code, '!message' => $response->status_message),
      WATCHDOG_ERROR
    );
    return FALSE;
  }
}
