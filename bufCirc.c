#include <bufCirc.h>

struct bufferCircular bufCirc;

void init_bufCirc(struct bufferCircular *buffer)
{
	buffer->first = 0;
	buffer->last = 0;
	buffer->estatBuf = BUIT;
}

//Afegeix c al bufCirc, si estava BUIT, passa a estar a DISPONIBLE o PLE. Si c és l'últim caràcter 
//que hi cap, s'afegeix i el buffer queda PLE. Si el buffer estava PLE prèviament, afegir no fa res. 
void afegir(struct bufferCircular *buffer, char c)
{
	if(buffer->estatBuf == BUIT) 
	{
		buffer->buf[buffer->last] = c;
		if(sizeBuf(buffer) >= MIDA) buffer->estatBuf = PLE;
		else buffer->estatBuf = DISPONIBLE;
	}
	else if(buffer->estatBuf == DISPONIBLE)
	{
		buffer->last = (buffer->last + 1) % MIDA;
		buffer->buf[buffer->last] = c;
		if(sizeBuf(buffer) >= MIDA) buffer->estatBuf = PLE;
	}
}

//Extraiem un element del bufCirc, l'eliminem del vector i el retornem. Si el buffer estava BUIT retornem 0, 
//si en canvi estava PLE, ara passarà a estar DISPONIBLE. 
char extreure(struct bufferCircular *buffer)
{
	int escollit;
	//Cas en que el buffer està buit
	if(buffer->estatBuf == BUIT) return 0;
	//Cas en que hi ha un sol element al buffer
	if(buffer->first == buffer->last)
	{
		buffer->estatBuf = BUIT;
		return buffer->buf[buffer->last];
	}
	else
	{
		escollit = (buffer->first) % MIDA;
		buffer->first = (escollit + 1) % MIDA;
		if (buffer->estatBuf == PLE) buffer->estatBuf = DISPONIBLE;
		return buffer->buf[escollit];
	}
	return 0;
}

//Possible upgrade d'eficiència: amb un switch
int sizeBuf(struct bufferCircular *buffer)
{	
	if(buffer->estatBuf == PLE) return MIDA; //Cas ple
	if(buffer->estatBuf == BUIT) return 0; //Cas buit
	if(buffer->first == buffer->last) return 1; //Cas un element
	//Resta de casos
	if(buffer->first < buffer->last) return (buffer->last - buffer->first);
	if(buffer->first > buffer->last) return (buffer->last + 1 + MIDA - buffer->first);
	return 0;
}


