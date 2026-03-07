# SPDX-License-Identifier: GPL-2.0+
# Copyright (C) 2025 Texas Instruments Incorporated - https://www.ti.com/
# Written by T Pratham <t-pratham@ti.com>
#
# Entry-type module for key file for signing images
#

from binman.etype.blob_named_by_arg import Entry_blob_named_by_arg

class Entry_key_path(Entry_blob_named_by_arg):
    """Key file path for signing images

    Properties / Entry arguments:
        - key-path: Full path with filename of the key file to read into the entry.

    This entry holds the full path to the key file used for signing images.
    """
    def __init__(self, section, etype, node):
        super().__init__(section, etype, node, 'key')
        self.external = True
