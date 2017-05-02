<?php

/**
 * @file
 * Theme template for the DataCite metadata record.
 */

<?xml version="1.0" encoding="UTF-8"?>
<resource xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns="http://datacite.org/schema/kernel-4" xsi:schemaLocation="http://datacite.org/schema/kernel-4 http://schema.datacite.org/meta/kernel-4/metadata.xsd">
  <!-- Required -->
  <identifier identifierType="DOI">10.5072/D3P26Q35R-Test</identifier>
  <!-- Required -->
  <creators>
    <?php foreach($creators as $creator): ?>
    <creator>
      <creatorName><?php print $creator; ?></creatorName>
    </creator>
      <?php endforeach; ?>
  </creators>
  <!-- Required -->
  <titles>
    <?php foreach($titles as $title): ?>
    <title><?php print $title; ?></title>
    <?php endforeach; ?>
  </titles>
  <!-- Required -->
  <publisher><?php print $publisher; ?></publisher>
  <!-- Required -->
  <publicationYear><?php print $publication_year; ?></publicationYear>
  <?php if (count($subjects)): ?>
  <subjects>
    <?php foreach($subjects as $subject): ?>
    <subject><?php print $subject; ?></subject>
    <?php endforeach; ?>
  </subjects>
  <?php endif; ?>
  <?php if ($language): ?>
    <language><?php print $language; ?></language>
  <?php endif; ?>
  <!-- Required -->
  <resourceType resourceTypeGeneral="<?php print $resource_type; ?>"><?php print $resource_type; ?></resourceType>
  <?php if ($version): ?>
  <version><?php print $version; ?></version>
  <?php endif; ?>
  <?php if (count($descriptions)): ?>
  <descriptions>
    <?php foreach($descriptions as $description): ?>
    <description descriptionType="Abstract">
    <?php endforeach; ?>
    </description>
  </descriptions>
  <?php endif; ?>
</resource>
