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
function islandora_doi_datacite_mods_get_dc_values($xml) {
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
function islandora_doi_datacite_mods_post_new($pid) {
  $symbol = variable_get('islandora_doi_datacite_mods_username', 'CISTI.FOO');
  $password = variable_get('islandora_doi_datacite_mods_password', '');
  $suffix_source = variable_get('islandora_doi_datacite_mods_suffix_source', 0);
  if ($suffix_source === 0) {
    $doi = variable_get('islandora_doi_datacite_mods_prefix', '10.99999/') . $pid;
  }
  if ($suffix_source === 1) {
    $uuid = islandora_doi_datacite_mods_get_uuid();
    $doi = variable_get('islandora_doi_datacite_mods_prefix', '10.99999/') . $uuid;
  }

  // POST the metadata first. This is a requirement of the API.
  // The DOI for the resource must be encoded in the XML in the
  // <identifier identifierType="DOI"> element.
  $api_url = variable_get('islandora_doi_datacite_mods_api_url', 'https://mds.datacite.org/') . 'metadata';

  $object = islandora_object_load($pid);
  $dc_values = islandora_doi_datacite_mods_get_dc_values($object['DC']->content);

  // Check for required fields: creator, title, publisher, publication_year,
  // resource_type, and that there is a YYYY value in date..
  $required_values = array('creator', 'title', 'publisher', 'date', 'type');
  foreach ($required_values as $value) {
    if (!isset($dc_values[$value]) || !count($dc_values[$value])) {
      drupal_set_message(t("Can't assign DOI to object !pid because its DC is missing a required value, \"!value\".", array('!pid' => $pid, '!value' => $value)), 'error');
      return FALSE;
    }
  }
  $year_present = FALSE;
  if (isset($dc_values['date'])) {
    foreach ($dc_values['date'] as $date) {
      if (preg_match('/(\d\d\d\d)/', $date, $matches)) {
        $year = $matches[1];
        $year_present = TRUE;
        break;
      }
    }
  }
  if (!$year_present) {
    drupal_set_message(t("Can't assign DOI to object !pid because its DC is missing a required value, !value.", array('!pid' => $pid, '!value' => $value)), 'error');
    return FALSE;
  }

  // resourceType must be from the list Audiovisual, Collection, Dataset,
  // Event, Image, InteractiveResource, Model, PhysicalObject, Service,
  // Software, Sound, Text, Workflow, Other.
  if (isset($dc_values['type'])) {
    $dc_values['type'] = islandora_doi_datacite_mods_normalize_resourcetype($dc_values['type']);
  }

  $metadata_xml = theme('datacite_metadata', array(
    'doi' => $doi,
    'creators' => isset($dc_values['creator']) ? $dc_values['creator'] : array(),
    'titles' => isset($dc_values['creator']) ? $dc_values['title'] : array(),
    'publishers' => isset($dc_values['publisher']) ? $dc_values['publisher'] : array(),
    'publication_years' => isset($dc_values['date']) ? $dc_values['date'] : array(),
    'subjects' => isset($dc_values['subject']) ? $dc_values['subject'] : array(),
    'languages' => isset($dc_values['language']) ? $dc_values['language'] : array(),
    'resource_types' => isset($dc_values['type']) ? $dc_values['type'] : array(),
    'descriptions' => isset($dc_values['description']) ? $dc_values['description'] : array(),
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
    watchdog(
      'islandora_doi_datacite_mods',
      'Error registering metadata for object !pid, so cannot proceed: !code, !message',
      array(
        '!pid' => $pid,
        '!code' => $response->code,
        '!message' => $response->status_message,
      ),
      WATCHDOG_ERROR
    );
    return FALSE;
  }

  // Then POST the resource's URL to mint the DOI.
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
      array(
        '!doi' => $doi,
        '!pid' => $pid,
        '!code' => $response->code,
        '!message' => $response->status_message),
      WATCHDOG_ERROR
    );
    return FALSE;
  }
}

/**
 * Replaces values in DC type element with preferred values.
 *
 * @param array $resourcetypes
 *   The array of dc.type values.
 *
 * @return array
 *   The normalized array of dc.type values.
 */
function islandora_doi_datacite_mods_normalize_resourcetype($resourcetypes) {
  $replacements = variable_get('islandora_doi_datacite_mods_resourcetype_replacements', "StillImage|Image\nThesis|Text");
  $replacements_array = preg_split('/\r\n|\n|\r/', $replacements, -1, PREG_SPLIT_NO_EMPTY);
  foreach ($resourcetypes as &$resourcetype) {
    foreach ($replacements_array as $replacement) {
      list($before, $after) = explode('|', $replacement);
      $resourcetype = preg_replace('/' . $before . '/', trim($after), $resourcetype);
    }
  }
  return $resourcetype;
}

/**
 * Very lazy generation of a UUID.
 *
 * @return string
 *   A UUID.
 */
function islandora_doi_datacite_mods_get_uuid() {
  return shell_exec('uuidgen');
}

