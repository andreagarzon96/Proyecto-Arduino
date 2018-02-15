<?php
require 'db_bootstrap.php';

$json = _json_decode(file_get_contents('php://input'));

if ($json != null)
{
  // Prepared Statement
  $stmt = $conn->prepare("INSERT INTO respuestas (arduino_id, pregunta_id, respuesta) VALUES (?, ?, ?)");
  $stmt->bind_param("sii", $arduino_id, $pregunta_id, $respuesta);

  $arduino_id = $json["ID"];
  $pregunta_id = $json["pregunta"];
  $respuesta = $respuesta["respuesta"];

  if($stmt->execute())
  {
    $response["code"] = 0;
    $response["message"] = "OK";
    echo json_encode($response);
  } else {
    $response["code"] = 1;
    $response["message"] = "STMT EXEC FAIL";
    echo json_encode($response);
  }

  // Cerrado del Prepared Statement
  $stmt->close();
} else {
  $response["code"] = 2;
  $response["message"] = "JSON DATA ERR";
  echo json_encode($response);
}
// Cerrar la conexion
$conn->close();

function _json_decode($string)
{
  if (get_magic_quotes_gpc())
  {
    $string = stripslashes($string);
  }
  return json_encode($string, true);
}
?>
