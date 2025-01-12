# gpmf-json-exporter
Uses the GoPro library to export GPMF data as JSON

## goal of this project
This project allows to write all MP4-GPMF content to be written to console as JSON. Via
different command line parameters you get a JSON stream, an array or can concatenate in
a existing file. 

I decided to use the command line as output to allow different use cases. Using a direct
file export would limit the use cases and would require write access to the filesystem.
