<?php

/**
 * @file
 * Utility functions.
 */


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
 * @param array $form
 *   The islandora_doi_framework_manage_doi form.
 * @param array $form_state
 *   The form state of islandora_doi_framework_manage_doi form on submission.
 *
 * @return string|bool
 *   The DOI if the request was successful, FALSE if not.
 */
function islandora_doi_datacite_post_new($pid, $form, $form_state) {
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

  $metadata_xml = islandora_doi_datacite_generate_metadata($pid, $doi, $form_state);

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
 * Updates an object's DOI with updated metadata and/or URL.
 *
 * @param string $pid
 *   The object's PID.
 * @param string $doi
 *   The object's DOI.
 *
 * @return bool
 *   TRUE if the DOI was updated, FALSE if not.
 */
function islandora_doi_datacite_update_doi($pid, $doi, $form_state) {
  $symbol = variable_get('islandora_doi_datacite_username', 'CISTI.FOO');
  $password = variable_get('islandora_doi_datacite_password', '');

  $api_url = variable_get('islandora_doi_datacite_api_url', 'https://mds.datacite.org/') . 'metadata';

  $metadata_xml = islandora_doi_datacite_generate_metadata($pid, $doi, $form_state);

  // DataCite's API uses POST to update the metadata.
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
      // The DataCite API provides specific error messages in the
      // response body.
      $message = $response->status_message . ': ' . $response->data;
    }
    else {
      $message = $response->status_message;
    }
    watchdog(
      'islandora_doi_datacite',
      'Error updating metadata for object !pid: !code, !message',
      array(
        '!pid' => $pid,
        '!code' => $response->code,
        '!message' => $message,
      ),
      WATCHDOG_ERROR
    );
    drupal_set_message(t("Can't update DataCite metadata for object !pid. See the Drupal log for more information.", array('!pid' => $pid)), 'error');
    return FALSE;
  }

  // Update the URL of an object associated with a DOI. Assumes the object
  // is on the same server as the old URL.
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
    drupal_set_message(t('URL for DOI !doi successfully updated (object !pid)', array('!doi' => $doi, '!pid' => $pid)));
    return $doi;
  }
  else {
    if ($response->code == '400') {
      // The DataCite API provides specific error messages in
      // the response body.
      $message = $response->status_message . ': ' . $response->data;
    }
    else {
      $message = $response->status_message;
    }
    watchdog(
      'islandora_doi_datacite',
      'Error updating URL associated with DOI !doi (object !pid): !code, !message',
      array(
        '!doi' => $doi,
        '!pid' => $pid,
        '!code' => $response->code,
        '!message' => $message),
      WATCHDOG_ERROR
    );
    drupal_set_message(t("Can't update URL for DOI !doi (object !PID). See the Drupal log for more information.", array('!pid' => $pid)), 'error');
    return FALSE;
  }
}

/**
 * Generates the DataCite metadata XML.
 *
 * @param string $pid
 *   The PID of the object to generate the metadata for.
 * @param string $doi
 *   The DOI of the object to generate the metadata for.
 *
 * @return string
 *   The DataCite metadata XML.
 */
function islandora_doi_datacite_generate_metadata($pid, $doi, $form_state) {
  $object = islandora_object_load($pid);
  $dc_values = islandora_doi_datacite_get_dc_values($object['DC']->content);

  $metadata_xml = theme('datacite_metadata', array(
    'doi' => $doi,
    'dc_values' => $dc_values,
    'title' => $form_state['values']['islandora_doi_datacite_title'],
    'creators' => $form_state['values']['islandora_doi_datacite_creator'],
    'publication_year' => $form_state['values']['islandora_doi_datacite_date'],
    'publisher' => $form_state['values']['islandora_doi_datacite_publisher'],
    'resource_type' => $form_state['values']['islandora_doi_datacite_resourcetype'],
  ));

  drupal_alter('islandora_doi_datacite_metadata_xml', $metadata_xml, $pid, $form_state);
  return $metadata_xml;
}

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
 * Very lazy generation of a UUID.
 *
 * @return string
 *   A UUID.
 */
function islandora_doi_datacite_get_uuid() {
  return shell_exec('uuidgen');
}
