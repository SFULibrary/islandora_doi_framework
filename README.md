# Islandora DOI Framework

## Overview

Utility module that provides a framework for other modules to assign DOIs ([Digital Object Identifiers](https://en.wikipedia.org/wiki/Digital_object_identifier)) to objects. This module provides the following:

* a "DOI" subtab under each object's "Mangage" tab
* four Drupal hooks
  * one for minting a DOI using an external API
  * one for persisting a DOI, for example in a datastream or database table
  * one for updating a DOI
  * one for checking for the presence of a DOI in a datastream or other location
* a "Assign DOIs to Islandora objects" permission
* a Drush script for adding a DOI to a list of objects

This module differs from the [Islandora DOI](https://github.com/Islandora/islandora_scholar/tree/7.x/modules/doi) module bundled with Islandora Scholar in that this module and its submodules only mint new DOIs and manage updating the data associated with a DOI. Scholar's DOI module allows for the creation of new objects from a list of DOIs.

## Requirements

* [Islandora](https://github.com/Islandora/islandora)
* A submodule to mint the DOIs, such as the included [DataCite](modules/islandora_doi_datacite) module
* A submodule to persist the DOI locally, such as the included [MODS](modules/islandora_doi_mods) module

## Installation

Same as for any other Drupal module.

## Configuration

This module does not have any configuration settings of its own. All settings are managed by submodules. It does provide a single permission, "Assign DOIs to Islandora objects", which enables users to access the "DOI" subtab under each object's "Manage" tab.

## Submodules

As described above, submodules are responsible for minting (generating) a DOI (typically, via an API provided by an external organization), for persisting it (typically in a datastream in each object), and for performing any updates to the metadata or URL associated with the DOI. One or more submodules together handle the combination of tasks required to mint a DOI from a specific source and then to persist it in a specific place associated with the Islandora object. The Islandora DOI Framework module defines hooks for accomplishing each of those tasks. These hooks are documented in the `islandora_doi_framework.api.php` file and are illustrated in the included [DataCite](modules/islandora_doi_datacite) and [MODS](modules/islandora_doi_mods) submodules. Note that all hooks do not need to be implemented in the same module; in fact, separating the DOI minting code from the DOI persiting code in separate modules is preferred so more combinations can be deployed.

To achieve those tasks, submodules will need to provide and manage whatever configuration settings they need, such as API endpoint URLs, API keys, etc.

Two submodules are available that can be used during the development and testing of new minting and persisting modules:

* A submodule to mint sample DOIs using a dummy DOI prefix, [Islandora DOI Framework Sample Mint](modules/islandora_doi_framework_sample_mint)
* A submodule to persist DOIs to a text file, [Islandora DOI Framework Sample Persist](modules/islandora_doi_framework_sample_persist)

Note that you should only enable a single minting submodule and a single persisting submodule. Therefore, if you enable one or both of these sample submodules, be sure to disable them before enabling your production minting and persisting submodules.

## Assigning DOIs to lists of objects

This module provides a Drush command to assign DOIs to a list of objects identified in an input file. The PID file is a simple list of object PIDS, one PID per line, like this:

```
islandora:23
islandora:29
// Comments can be prefixed by // or #.
islandora:107
islandora:2183
```

The command (using a file at `/tmp/pids.txt` containing the above list) is:

`drush islandora_doi_framework_assign_dois --user=admin --objects=/tmp/pids.txt`

## Maintainer

* [Mark Jordan](https://github.com/mjordan)

## Development and feedback

Pull requests against this module are welcome, as are submodules (suggestions below). Please open an issue in this module's Github repo before opening a pull request.

Submodules that mint DOIs from other [registration agencies](http://www.doi.org/registration_agencies.html) are particularly welcome, as are submodules for persisting DOIs to non-MODS datastreams or other locations.

## To do

* Complete the Drush script used to assign batches of DOIs.
* Add the ability to update DataCite metadata and object URLs associated with DOIs. Figure out best trigger and workflow for updating metadata. This should probably not happen every time the source datastream is modified, although that is one option. Maybe a second button under the "DOI" tab for updating metadata? Could also have an associated drush commnand.

## License

 [GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)

