
indexHtml = dir -> (
     if not isDirectory dir then error "expected a directory";
     title := baseFilename dir;
     ind := minimizeFilename (dir|"/index.html");
     if fileExists ind then (
	  if not match("generated by indexHtml",get ind) then error("file not made by indexHtml already present: ",ind);
	  );
     ind = openOut ind;
     ind << ///
<?xml version="1.0" encoding="us-ascii"?>
<!-- generated by indexHtml -->
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<title>///
<< title
<< ///</title>
</head>

<body>
<h1>/// 
     << title 
     << ///</h1>///;
     scan(readDirectory dir, fn -> (
	       if fn == "." or fn == ".." then return;
	       fn2 := minimizeFilename(dir|"/"|fn);
	       if isDirectory fn2 then indexHtml fn2
	       else (
		    ind << ///<li><A HREF="///
		    << fn
		    << ///">///
		    << fn
		    << ///</A>///;
		    if isRegularFile fn2 then (
			 ind << " (" << fileLength fn2 << " bytes)";
			 );
		    ind << "</li>" << endl)));
     ind << ///
</ul>
<hr>
<ul>
  <li><a href="http://validator.w3.org/check/referer">Validate</a> the html on this page.</li>
</ul>
</body>
/// 
     << close;
     )
