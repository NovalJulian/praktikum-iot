<?php
include "koneksi.php";

if (isset($_POST['suhu']) && isset($_POST['kelembapan'])) {

    $suhu = floatval($_POST['suhu']);
    $kelembapan = floatval($_POST['kelembapan']);

    mysqli_query($conn,
        "INSERT INTO data (suhu, kelembapan)
         VALUES ('$suhu', '$kelembapan')"
    );

    echo "OK";
} else {
    echo "ERROR";
}
?>
