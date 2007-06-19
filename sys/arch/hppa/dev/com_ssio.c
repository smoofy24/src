/*	$OpenBSD: com_ssio.c,v 1.1 2007/06/19 22:51:26 kettenis Exp $	*/

/*
 * Copyright (c) 2007 Mark Kettenis
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/device.h>
#include <sys/tty.h>

#include <machine/bus.h>

#include <dev/ic/comreg.h>
#include <dev/ic/comvar.h>

#include <hppa/dev/ssiovar.h>

#define COM_SSIO_FREQ	1843200

int com_ssio_match(struct device *, void *, void *);
void com_ssio_attach(struct device *, struct device *, void *);

struct cfattach com_ssio_ca = {
	sizeof(struct com_softc), com_ssio_match, com_ssio_attach
};

int
com_ssio_match(struct device *parent, void *match, void *aux)
{
	struct cfdata *cf = match;
	struct ssio_attach_args *saa = aux;

	if (strcmp(saa->saa_name, "com") != 0)
		return (0);

	/* Check locators. */
	if (cf->ssiocf_irq != SSIO_UNK_IRQ && cf->ssiocf_irq != saa->saa_irq)
		return (0);

	return (1);
}

void
com_ssio_attach(struct device *parent, struct device *self, void *aux)
{
	struct com_softc *sc = (void *)self;
	struct ssio_attach_args *saa = aux;

	sc->sc_iot = saa->saa_iot;
	sc->sc_iobase = saa->saa_iobase;
	if (bus_space_map(sc->sc_iot, sc->sc_iobase, COM_NPORTS,
	    0, &sc->sc_ioh)) {
		printf(": cannot map io space\n");
		return;
	}

	sc->sc_frequency = COM_SSIO_FREQ;
	com_attach_subr(sc);

	sc->sc_ih = ssio_intr_establish(IPL_TTY, saa->saa_irq,
	    comintr, sc, sc->sc_dev.dv_xname);
}
