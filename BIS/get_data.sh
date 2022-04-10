#!/bin/bash

filename="full_bis_cbs_csv.zip"
wget "https://www.bis.org/statistics/$filename"
unzip "$filename"
rm "$filename"


