<?php

function displayDir($dirName) {


    echo "<table border=\"4\" cellpadding=\"2\">";
    echo "<caption style='font-weight: bold; font-size: 1.5em'>$dirName</caption>";
    echo "<tr>";
    echo "<th>HTML</th>";
    echo "<th>PDF</th>";
    echo "<th>TXT</th>";
    echo "</tr>";

    $dir = opendir($dirName);
    while (($file = readdir($dir)) !== false) {
        if ($file == "." || $file == ".." || $file == "index.php")
            continue;
        
        if (substr($file, strpos($file, '.') + 1, strlen($file)) != "html")
            continue;
        
        $htmlfileName =  $file;
        $htmlfile = $dirName . "/" . $htmlfileName;
        echo "<tr>";
        echo "<td><a href=\"$htmlfile\">$htmlfileName</a></td>";
        
        $pdffileName =  basename($file, ".html") . ".pdf";
        $pdffile = $dirName . "/" . $pdffileName;
        if (file_exists("${pdffile}"))
            echo "<td><a href=\"$pdffile\">$pdffileName</a></td>";
        else
            echo "<td>$pdffile</td>";

        $txtfileName = basename($file, ".html") . ".txt";
        $txtfile = $dirName . "/" . $txtfileName;
        if (file_exists($txtfile))
            echo "<td><a href=\"$txtfile\">$txtfileName</a></td>";
        else
            echo "<td></td>";
        
        echo "<tr>";
    }
    closedir($dir);

    echo "</table>";

}
	 
$title = "Documentation";
	 
echo "<html><title>$title</title><body>";
echo "<h1>$title</h1>";
	 

$modules = array("strmsrv", "v2dlib", "V2O", "player");
foreach ($modules as $module) {
    displayDir($module);
    echo "<br/><br/>";
}


echo "</body></html>";
	 
?>
