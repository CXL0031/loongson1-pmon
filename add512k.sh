#!/bin/bash
cp $1 /tmp
printf "%0.s\xff" {1..524288} >>/tmp/$1
dd if=/tmp/$1 of=$1.512k count=1 bs=524288
