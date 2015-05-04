TEMPLATE = subdirs

SUBDIRS += \
	triangulator \
	surfaceshader \
	app

app.depends = triangulator surfaceshader
