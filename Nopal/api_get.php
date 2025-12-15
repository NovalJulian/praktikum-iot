<?php
include "koneksi.php";

$q = mysqli_query($conn, "SELECT * FROM data ORDER BY id DESC LIMIT 1");

if ($q && mysqli_num_rows($q) > 0) {
    echo json_encode(mysqli_fetch_assoc($q));
} else {
    echo json_encode([]);
}
?>
