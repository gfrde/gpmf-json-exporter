# gpmf-json-exporter
Uses the GoPro library to export GPMF data as JSON

## goal of this project
This project allows to write all MP4-GPMF content to be written to console as JSON. Via
different command line parameters you get a JSON stream, an array or can concatenate in
a existing file. 

I decided to use the command line as output to allow different use cases. Using a direct
file export would limit the use cases and would require write access to the filesystem.

## examples

### export main data stream

#### as complete json file
```
gpmf-json-exporter my-video.MP4  -a -j -ja -f > my-video.json
```

#### as json stream data (line based export)
```
gpmf-json-exporter my-video.MP4  -a -j -js -f > my-video.jsonl
```


### export file meta data
the option '-m' uses the MP4 meta data as source

```
gpmf-json-exporter my-video.MP4  -a -j -ja -m -f > my-video-meta.json
```
