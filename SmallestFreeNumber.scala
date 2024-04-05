import scala.io.Source


def readFile(filename: String): Unit = {

  val bufferedSource = Source.fromFile(filename)
  var data = new ListBuffer[String]()

  for (line <- bufferedSource.getLines) {
    val values = line.split(",").map(_.trim)
    values.foreach(data.::) 
  }
  for (x <- data) {
    print(x)
  }
}


@main def SmallestFreeNumber(args: String*): Unit = {
  readFile("10000.csv")
}


