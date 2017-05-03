<?php

/**
 * @file
 * Theme template for the DataCite metadata record.
 */
?>
<resource xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns="http://datacite.org/schema/kernel-4" xsi:schemaLocation="http://datacite.org/schema/kernel-4 http://schema.datacite.org/meta/kernel-4/metadata.xsd">
  <identifier identifierType="DOI"><?php print $doi; ?></identifier>
  <creators>
  <?php foreach($creators as $creator) : ?>
    <creator>
      <creatorName><?php print $creator; ?></creatorName>
    </creator>
  <?php endforeach; ?>
  </creators>
  <titles>
  <?php foreach($titles as $title): ?>
    <title><?php print $title; ?></title>
  <?php endforeach; ?>
  </titles>
  <?php foreach($publishers as $publisher): ?>
  <publisher><?php print $publisher; ?></publisher>
  <?php endforeach; ?>
  <?php foreach($publication_years as $publication_year): ?>
  <publicationYear><?php print $publication_year; ?></publicationYear>
  <?php endforeach; ?>
  <?php if (isset($subjects) && count($subjects)): ?>
  <subjects>
  <?php foreach($subjects as $subject): ?>
    <subject><?php print $subject; ?></subject>
  <?php endforeach; ?>
  </subjects>
  <?php endif; ?>
  <?php if (isset($languages) && count($languages)): ?>
    <?php foreach($languages as $language): ?>
    <language><?php print $language; ?></language>
    <?php endforeach; ?>
  <?php endif; ?>
  <?php foreach($resource_types as $resource_type): ?>
  <resourceType resourceTypeGeneral="<?php print $resource_type; ?>"><?php print $resource_type; ?></resourceType>
  <?php endforeach; ?>
  <?php if (isset($descriptions) && count($descriptions)): ?>
  <descriptions>
    <?php foreach($descriptions as $description): ?>
    <description descriptionType="Abstract">
    <?php endforeach; ?>
    </description>
  </descriptions>
  <?php endif; ?>
</resource>
