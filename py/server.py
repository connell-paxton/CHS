import spam

def respond(fd, req):
	print(fd)
	spam.write(3, "HTTP/1.1	200 OK\r\n\r\nHello")

spam.server("90", spam.serverinit)
