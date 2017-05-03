# Islandora DOI Framework DataCite/MODS

## Overview

Submodule of the Islandora DOI framework module that manages DOIs provided by [DataCite](https://www.datacite.org/) and persists them to objects' MODS datastream.

This module creates a record complying with the [DataCite Metadata Schema](https://schema.datacite.org/) from an object's DC datastream, and using the "Assign DOI" functionality provided by the Islandora DOI Framework module, posts it to the DataCite Metadata Store along with the object's URL. These two tasks together mint a DOI for the object. The object's PID is used as its DOI's "suffix", resulting in DOIs that look like 10.5072/islandora:1234 ('10.5072' is the test DOI prefix; the one assigned to your institution will be used instead).

Note that the DataCite Metadata Schema enforces some constraints. Specifically, the schema requires what are the equivalents of DC's 'creator', 'title', 'publisher', 'date', and 'type'. Also, the 'date' must be a year (yyyy). If a user of this module tries to assign a DOI for an object that doesn't meet these metadata, they are told that the object is missing a required metadata value.

## Requirements

* Islandora
* [Islandora DOI Framework](../..)

## Installation

Same as for any other Drupal module.

## Configuration

Go to `admin/islandora/tools/islandora_doi_datacite_mods` to enter your DataCite institutional symbol and password, and your institution's DOI prefix (which will be assigned to you by DataCite).

DataCite's metadata schema requires that the values used in its 'resourceType' elememts are from the list 'Audiovisual', 'Collection', 'Dataset', 'Event', 'Image', 'InteractiveResource', 'Model', 'PhysicalObject', 'Service', 'Software', 'Sound', 'Text', 'Workflow', and 'Other'. You can map DC.type values used in your repository to the required DataCite values using a list of source|replacement pairs here that maps values in your DC datastream's 'type' element to one of these values. Place each source and replacement value pair, separated by a |, on its own line. Two pairs are provided as examples:

```
StillImage|Image
Thesis|Text
```
Using these default replacement pairs as an example, if an object's DC.type element has a value of "Thesis", the DataCite metadata record will get a "resourceType" value of "Text".

This module also provides the option of using an object's PID as the DOI suffix or using a UUID (version 4). PIDs are specific to an Islandora instance, while UUIDs are globally unique. PIDs make completely suitable DOI suffixes, but should you migrate to another platform in the future, your DOIs would still contain Islandora PIDs.


## Maintainer

* [Mark Jordan](https://github.com/mjordan)

## Development and feedback

Pull requests are welcome, as are use cases and suggestions.

## License

 [GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)

