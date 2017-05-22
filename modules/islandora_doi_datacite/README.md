# Islandora DOI DataCite

## Overview

Submodule of the Islandora DOI framework module that manages DOIs provided by [DataCite](https://www.datacite.org/).

This module creates a record complying with the [DataCite Metadata Schema](https://schema.datacite.org/) from an object's DC datastream, and using the "Assign DOI" functionality provided by the Islandora DOI Framework module, posts it to the [DataCite Metadata Store](https://search.datacite.org/) along with the object's URL. These two tasks together mint a DOI for the object. The object's PID is used as its DOI's "suffix", resulting in DOIs that look like 10.5072/islandora:1234 ('10.5072' is the test DOI prefix; the one assigned to your institution will be used instead). Optionally, the module can generate a UUID to use as the DOI suffix.

This module also provides the ability to update the meatadata and URL associated with a DOI. If an object has a DOI, the user is presented with an "Update DOI" button instead of an "Assign DOI" button. Currently, updating DOIs needs to be done manually. Use cases for when automatic updates should be applied are welcome.

DataCite's Metadata Schema enforces some constraints. Specifically:

* the schema requires elements that correspond to DC's 'creator', 'title', 'publisher', 'date', and 'type'
* the date must be a year (yyyy)
* the type must be from a controlled list (details below).

If a user tries to assign a DOI for an object that doesn't meet these metadata, they are told that the object is missing one or more required DC metadata values.

## Requirements

* [Islandora](https://github.com/Islandora/islandora)
* [Islandora DOI Framework](../..)
* A submodule of the Islandora DOI Framework that persists DOIs, such as [Islandora DOI MODS](../islandora_doi_mods).

## Installation

Same as for any other Drupal module.

## Configuration

Go to `admin/islandora/tools/islandora_doi_datacite` to enter your DataCite institutional symbol and password, and your institution's DOI prefix (which will be assigned to you by DataCite).

DataCite's metadata schema requires that the values used in its 'resourceType' elememts are from the list 'Audiovisual', 'Collection', 'Dataset', 'Event', 'Image', 'InteractiveResource', 'Model', 'PhysicalObject', 'Service', 'Software', 'Sound', 'Text', 'Workflow', and 'Other'. You can map DC.type values used in your repository to the required DataCite values using a list of source|replacement pairs here that maps values in your DC datastream's 'type' element to one of these values. Place each source and replacement value pair, separated by a |, on its own line. Two pairs are provided as examples:

```
StillImage|Image
Thesis|Text
```
Using these default replacement pairs as an example, if an object's DC.type element has a value of "Thesis", the DataCite metadata record will get a "resourceType" value of "Text".

This module also provides the option of using an object's PID as the DOI suffix or using a UUID (version 4). PIDs are specific to an Islandora instance, while UUIDs are globally unique. PIDs make completely suitable DOI suffixes, but should you migrate to another platform in the future, your DOIs would still contain Islandora PIDs.

There is also an option to use both the object's DC.creator and DC.contributor values to populate DataCite's reqiured 'creator' element. Because of the way that the Library of Congress' MODS-to-DC stylesheet generates DC datastreams, many Islandora objects' DC datastreams contain 'contributor' elements rather than 'creator' elements. Enabling this option will reduce the number of validation failures based on the lack of values for the DataCite 'creator' element.


## Maintainer

* [Mark Jordan](https://github.com/mjordan)

## Development and feedback

Pull requests are welcome, as are use cases and suggestions.

## License

 [GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)

