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
 *   Not all DC elements are guaranteed to be present in the array.
 */
function islandora_doi_datacite_get_dc_values($xml) {
  $dc_values = array();
  $dom = new DOMDocument();
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
 * Also checks to make sure that the source DC datastream
 * contains the elements required by the DataCite metadata
 * schema.
 *
 * The resource's metadata must be registered via the DataCite MDS API
 * first, then its URL. See https://datacite.readme.io/docs/mds-2 for
 * additional info.
 *
 * @param string $pid
 *   The PID of the object.
 *
 * @return string|bool
 *   The DOI if the request was successful, FALSE if not.
 */
function islandora_doi_datacite_post_new($pid) {
  $symbol = variable_get('islandora_doi_datacite_username', 'CISTI.FOO');
  $password = variable_get('islandora_doi_datacite_password', '');
  $suffix_source = variable_get('islandora_doi_datacite_suffix_source', 'pid');
  if ($suffix_source == 'pid') {
    $doi = variable_get('islandora_doi_datacite_prefix', '10.99999/') . $pid;
    $doi = trim($doi);
  }
  if ($suffix_source == 'uuid') {
    $uuid = islandora_doi_datacite_get_uuid();
    $doi = variable_get('islandora_doi_datacite_prefix', '10.99999/') . $uuid;
    $doi = trim($doi);
  }

  // POST the metadata first. This is a requirement of the API.
  // The DOI for the resource must be encoded in the XML in the
  // <identifier identifierType="DOI"> element.
  $api_url = variable_get('islandora_doi_datacite_api_url', 'https://mds.datacite.org/') . 'metadata';

  $object = islandora_object_load($pid);
  $dc_values = islandora_doi_datacite_get_dc_values($object['DC']->content);

  if (variable_get('islandora_doi_datacite_combine_creator', 0)) {
    $dc_values['creator'] = isset($dc_values['creator']) ? $dc_values['creator'] : array();
    $dc_values['contributor'] = isset($dc_values['contributor']) ? $dc_values['contributor'] : array();
    $dc_values['creator'] = array_merge($dc_values['creator'], $dc_values['contributor']);
  }

  // Check for source DC elements (creator, title, publisher, date) that
  // correspond to the required DataCite elements creator, title, publisher,
  // publicationYear, and resourceType, and that there is a YYYY value in date.
  $required_values = array('creator', 'title', 'publisher', 'date', 'type');
  $missing_required_elements = array();
  foreach ($required_values as $value) {
    if (!isset($dc_values[$value]) || !count($dc_values[$value])) {
      $missing_required_elements[] = $value;
    }
  }
  if (count($missing_required_elements)) {
    $missing_elements_string = implode(', ', $missing_required_elements);
    drupal_set_message(t("Can't assign DOI to object !pid because its DC is missing one or more required values: !values.", array('!pid' => $pid, '!values' => trim($missing_elements_string))), 'error');
    return FALSE;
  }

  $year_present = FALSE;
  if (isset($dc_values['date'])) {
    foreach ($dc_values['date'] as $date) {
      if (preg_match('/(\d\d\d\d)/', $date, $matches)) {
        $year = trim($matches[1]);
        $year_present = TRUE;
        break;
      }
    }
  }
  if (!$year_present) {
    drupal_set_message(t("Can't assign DOI to object !pid because its DC.date value does not contain a year in yyyy format, !value.", array('!pid' => $pid, '!value' => $value)), 'error');
    return FALSE;
  }

  // resourceType must be Audiovisual, Collection, Dataset, Event, Image,
  // InteractiveResource, Model, PhysicalObject, Service, Software, Sound,
  // Text, Workflow, or Other.
  if (isset($dc_values['type'])) {
    $resource_type = islandora_doi_datacite_normalize_resourcetype($dc_values['type']);
  }

  $metadata_xml = theme('datacite_metadata', array(
    'doi' => $doi,
    'dc' => $dc_values,
    'publication_year' => $year,
    'resource_type' => $resource_type,
  ));

  $response = drupal_http_request($api_url, array(
    'headers' => array(
      'Content-Type' => 'application/xml;charset=UTF-8',
      'Authorization' => 'Basic ' . base64_encode($symbol . ':' . $password),
    ),
    'method' => 'POST',
    'data' => $metadata_xml,
  ));
  if ($response->code != 201) {
    if ($response->code == '400') {
      // The DataCite API provides specific error messages in the response body.
      $message = $response->status_message . ': ' . $response->data;
    }
    else {
      $message = $response->status_message;
    }
    watchdog(
      'islandora_doi_datacite',
      'Error registering metadata for object !pid, so cannot mint DOI: !code, !message',
      array(
        '!pid' => $pid,
        '!code' => $response->code,
        '!message' => $message,
      ),
      WATCHDOG_ERROR
    );
    drupal_set_message(t("Can't post DataCite metadata for object !pid. See the Drupal log for more information.", array('!pid' => $pid)), 'error');
    return FALSE;
  }

  // Then POST the resource's URL to mint the DOI.
  $api_url = variable_get('islandora_doi_datacite_api_url', 'https://mds.datacite.org/') . 'doi';
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
    drupal_set_message(t('DOI !doi successfully minted for object !pid', array('!doi' => $doi, '!pid' => $pid)));
    return $doi;
  }
  else {
    if ($response->code == '400') {
      // The DataCite API provides specific error messages in the response body.
      $message = $response->status_message . ': ' . $response->data;
    }
    else {
      $message = $response->status_message;
    }
    watchdog(
      'islandora_doi_datacite',
      'Error minting DOI !doi for object !pid: !code, !message',
      array(
        '!doi' => $doi,
        '!pid' => $pid,
        '!code' => $response->code,
        '!message' => $message),
      WATCHDOG_ERROR
    );
    drupal_set_message(t("Can't mint DOI for object !pid. See the Drupal log for more information.", array('!pid' => $pid)), 'error');
    return FALSE;
  }
}

/**
 * Replaces values in DC type element with preferred values.
 *
 * @param array $resourcetypes
 *   The array of dc.type values.
 *
 * @return string
 *   The first member of the normalized array of dc.type values.
 */
function islandora_doi_datacite_normalize_resourcetype($resourcetypes) {
  $replacements = variable_get('islandora_doi_datacite_resourcetype_replacements', "StillImage|Image\nThesis|Text");
  $replacements_array = preg_split('/\r\n|\n|\r/', $replacements, -1, PREG_SPLIT_NO_EMPTY);
  foreach ($resourcetypes as &$resourcetype) {
    foreach ($replacements_array as $replacement) {
      list($before, $after) = explode('|', $replacement);
      $resourcetype = preg_replace('/' . $before . '/', trim($after), $resourcetype);
    }
  }
  return $resourcetypes[0];
}

/**
 * Very lazy generation of a UUID.
 *
 * @return string
 *   A UUID.
 */
function islandora_doi_datacite_get_uuid() {
  return shell_exec('uuidgen');
}
