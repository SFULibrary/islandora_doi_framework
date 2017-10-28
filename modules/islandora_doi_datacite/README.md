# Islandora DOI DataCite

## Overview

Submodule of the Islandora DOI framework module that manages DOIs provided by [DataCite](https://www.datacite.org/).

This module creates a record complying with the [DataCite Metadata Schema](https://schema.datacite.org/) from an object's DC datastream, and using the "Assign DOI" functionality provided by the Islandora DOI Framework module, posts it to the [DataCite Metadata Store](https://search.datacite.org/) along with the object's URL. These two tasks together mint a DOI for the object. The object's PID is used as its DOI's "suffix", resulting in DOIs that look like 10.5072/islandora:1234 ('10.5072' is the test DOI prefix; the one assigned to your institution will be used instead). Optionally, the module can generate a UUID to use as the DOI suffix.

This module also provides the ability to update the meatadata and URL associated with a DOI. If an object has a DOI, the user is presented with an "Update DOI" button instead of an "Assign DOI" button. Currently, updating DOIs needs to be done manually. Use cases for when automatic updates should be applied are welcome.

DataCite's Metadata Schema enforces some constraints. Specifically:

* the schema requires elements that correspond to DC's 'creator', 'title', 'publisher', 'date', and 'type'
* the date must be a year (yyyy)
* the type must be from the following controlled list: 'Audiovisual', 'Collection', 'Dataset', 'Event', 'Image', 'InteractiveResource', 'Model', 'PhysicalObject', 'Service', 'Software', 'Sound', 'Text', 'Workflow', and 'Other'.

If a user tries to assign a DOI for an object that doesn't meet these metadata, they are told that the object is missing one or more required DC metadata values.

## Requirements

* [Islandora](https://github.com/Islandora/islandora)
* [Islandora DOI Framework](https://github.com/mjordan/islandora_doi_framework)
* A submodule of the Islandora DOI Framework that persists DOIs, such as [Islandora DOI MODS](../islandora_doi_mods).

## Installation

Same as for any other Drupal module.

## Configuration

Go to `admin/islandora/tools/islandora_doi_datacite` to enter your DataCite institutional symbol and password, and your institution's DOI prefix (which will be assigned to you by DataCite).

This module provides the option of using an object's PID as the DOI suffix or using a UUID (version 4). PIDs are specific to an Islandora instance, while UUIDs are globally unique. PIDs make completely suitable DOI suffixes, but should you migrate to another platform in the future, your DOIs would still contain Islandora PIDs.

There is also an option to use both the object's DC.creator and DC.contributor values to populate DataCite's reqiured 'creator' element. Because of the way that the Library of Congress' MODS-to-DC stylesheet generates DC datastreams, many Islandora objects' DC datastreams contain 'contributor' elements rather than 'creator' elements. Enabling this option will reduce the number of validation failures based on the lack of values for the DataCite 'creator' element.

## Assigning DataCite DOIs from a list of PIDs

This module includes a drush script that can assign DOIs from a file containing a list of PIDS. The PID file contains one PID per line, and lines can be commented:

```
islandora:10
islandora:11
example:5782
# This line will be ignored.
// So will this one.
islandora:948
someothernamespace:1
someothernamespace:2
```

The script provides two commands:

* `islandora_doi_datacite_assign_dois_preflight` and
* `islandora_doi_datacite_assign_dois`.

Configuration options set in the admin GUI as described above are used by the drush commands.

You should run the preflight command on your list of PIDs before running the assign command. The preflight command checks each object identified in the PID file to confirm that its DC datastream contains the values required by the  DataCite metadata schema, specifically, for a dc.title, dc.creator, dc.publisher. It also checks the dc.date field for a YYYY year. Running the file produces two output files, named after the PID file with `.passed` and `.errors` appended. The 'passed' file contains PIDs of objects that had all the required values, and the 'errors' file contains a log of the problems (missing required elements, DOI already exists, etc.) in each object. For example:

```
drush -u 1 islandora_doi_datacite_assign_dois_preflight --pid_file=/tmp/dois.pids
```

The 'passed' file can then be used as the input for the `islandora_doi_datacite_assign_dois` command, which assigns DOIs to each object listed in the PID file. Since this command kips objects that do not meet the requred DC values, using the 'passed' file is not required, but using it will mean that none of the PIDs listed in it will be skipped. The `islandora_doi_datacite_assign_dois` command requires the `--resource_type` option, whose value must be from the list above. For example:

```
drush -u 1 islandora_doi_datacite_assign_dois --pid_file=/tmp/dois.pids.passed --resource_type=Text
```

For example, given a PID file `/tmp/dois.pids.passed` that contains two PIDs `doitest:2` and `doitest:3`, running this command will assign the DOIs and produce output like this:

```
You are about to mint new DOIs. Have you run the preflight check? (y/n): y
DOI 10.5072/doitest:2 assigned to object doitest:2                                                                              [ok]
DOI 10.5072/doitest:3 assigned to object doitest:3                                                                              [ok]
DOI 10.5072/doitest:2 successfully minted for object doitest:2                                                                  [status]
DOI 10.5072/doitest:2 successfully saved in doitest:2's MODS datastream.                                                        [status]
DOI 10.5072/doitest:3 successfully minted for object doitest:3                                                                  [status]
DOI 10.5072/doitest:3 successfully saved in doitest:3's MODS datastream.                                                        [status]
```

## Maintainer

* [Mark Jordan](https://github.com/mjordan)

## Development and feedback

Pull requests are welcome, as are use cases and suggestions.

## To do

* Add the ability to update the DOIs of both a single object and a group of objects. Some use cases are available in issue #7.

## License

 [GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)

