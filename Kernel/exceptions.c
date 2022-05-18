
#define ZERO_EXCEPTION_ID 0

static void zero_division();

// Es la funcion que recibira el codigo de la excepcion y, con ello, llamar a su rutina de atencion
void exceptionDispatcher(int exception) {
	if (exception == ZERO_EXCEPTION_ID)
		zero_division();
}

// Esta funcion es la que se ejecutara al realizarse una division por cero (exceptionID = 0)
static void zero_division() {
	// Handler para manejar excepcíon
}