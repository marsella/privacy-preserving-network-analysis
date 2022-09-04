#!/bin/bash

filename="full_cbs_pub_csv.zip"
wget "https://www.bis.org/statistics/$filename"
unzip "$filename"
rm "$filename"


