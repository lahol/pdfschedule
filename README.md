# PdfSchedule #
## General information ##
PdfSchedule is intended to present a pdf presentation (containing a schedule)
and loop through a given number of pages. It is completely controlled via a
fifo at /tmp/pdfscheduleipc.

## Commands ##
### load ###
*load* _document_
Loads a document.

### reload ###
*reload*
Reloads the current document.

### pages ###
*pages* _pagenumber1_ _pagenumber2_ ...
Loops through the given pages.

### quit ###
*quit*
Quit the application.

### set ###
*set* _setting_ _value(s)_

## Settings ##
### fullscreen ###
*fullscreen* [true|false]
Sets fullscreen mode.

### size ###
*size* _width_ _height_
Render a page to a rectangle of this size.

### page-interval ###
*page-interval* _interval_
Set the time each page is shown in seconds. A value of 0 disables looping.

### clock-show ###
*clock-show* [true|false]
Display a clock.

### clock-position ###
*clock-position* _x_ _y_
Position in page coordinates of the clock.

### clock-color ###
*clock-color* _GdkRGBA-Spec_
Font color for the clock. (see https://developer.gnome.org/gdk3/stable/gdk3-RGBA-Colors.html#gdk-rgba-parse)

### clock-font ###
*clock-font* _pango-font-spec_
Set font and size of the clock.
