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

Go to `admin/islandora/tools/islandora_doi_datacite_mods` to enter your DataCite institutional symbol and password.

## Maintainer

* [Mark Jordan](https://github.com/mjordan)

## Development and feedback

Pull requests are welcome, as are use cases and suggestions.

## License

 [GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)

