# Islandora DOI

## Overview

Utility module that provides a framework for other modules to assign DOIs ([Digital Object Identifiers](https://en.wikipedia.org/wiki/Digital_object_identifier)) to objects. This module provides the following:

* an "Assign DOI" subtab under each object's "Mangage" tab
* two Drupal hooks
  * one for fetching a DOI from an external API
  * one for persisting it, for example in a datastream for database
* a "Assign DOIs to Islandora objects" permission
* a Drush script for adding a DOI to objects

## Requirements

* Islandora

## Installation

Same as for any other Drupal module.

## Configuration

This module does not have any configuration options of its own. All settings are managed by submodules.

## Submodules

As described above, other modules are responsible for getting a DOI from an external source (typically an API) and for persisting it (typically in a datastream in each object). The Islandora DOI module provides a hook for accomplishing each of those tasks. These hooks are documented in the `islandora_doi.api.php` file and are illustrated in the included submodule.

To achieve those tasks, submodules will need to provide and manage whatever configuration settings they need, such as API endpoint URLs, API keys, etc.

## Maintainer

* [Mark Jordan](https://github.com/mjordan)

## Development and feedback

Pull requests are welcome, as are use cases and suggestions.

## License

 [GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)

