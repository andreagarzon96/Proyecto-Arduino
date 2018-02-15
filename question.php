<?php
require 'db_bootstrap.php';

if (isset($_GET["id"]))
{
  $query = "SELECT * FROM preguntas WHERE id = " . $_GET["id"];
  $result = $conn->query($query);

  if ($result->num_rows > 0)
  {
    while ($row = $result->fetch_assoc())
    {
      echo $row["pregunta"];
    }
  } else {
    echo "No existe pregunta.";
  }
}

$conn->close();

?>
