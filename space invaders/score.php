<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $score = $_POST['score'];
    $file = 'scores.txt';
    $currentScores = file_get_contents($file);
    $currentScores .= "Score: $score\n";
    file_put_contents($file, $currentScores);
    echo "Score enregistré avec succès!";
}
?>