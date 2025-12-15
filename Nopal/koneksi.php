<?php
$conn = mysqli_connect("localhost", "root", "", "nopal");

if (!$conn) {
    die("Koneksi gagal: " . mysqli_connect_error());
}
?>
