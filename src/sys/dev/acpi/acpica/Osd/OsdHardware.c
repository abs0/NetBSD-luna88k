/*	$NetBSD: OsdHardware.c,v 1.9 2004/04/11 06:52:38 kochi Exp $	*/

/*
 * Copyright 2001 Wasabi Systems, Inc.
 * All rights reserved.
 *
 * Written by Jason R. Thorpe for Wasabi Systems, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed for the NetBSD Project by
 *	Wasabi Systems, Inc.
 * 4. The name of Wasabi Systems, Inc. may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY WASABI SYSTEMS, INC. ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL WASABI SYSTEMS, INC
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * OS Services Layer
 *
 * 6.7: Address Space Access: Port Input/Output
 * 6.8: Address Space Access: Memory and Memory Mapped I/O
 * 6.9: Address Space Access: PCI Configuration Space
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: OsdHardware.c,v 1.9 2004/04/11 06:52:38 kochi Exp $");

#include <sys/param.h>
#include <sys/device.h>

#include <dev/acpi/acpica.h>
#include <dev/acpi/acpivar.h>

#include <machine/acpi_machdep.h>

/*
 * ACPICA doesn't provide much in the way of letting us know which
 * hardware resources it wants to use.  We therefore have to resort
 * to calling machinde-dependent code to do the access for us.
 */

/*
 * AcpiOsReadPort:
 *
 *	Read a value from an input port.
 */
ACPI_STATUS
AcpiOsReadPort(ACPI_IO_ADDRESS Address, UINT32 *Value, UINT32 Width)
{

	switch (Width) {
	case 8:
		*Value = acpi_md_OsIn8(Address);
		break;

	case 16:
		*Value = acpi_md_OsIn16(Address);
		break;

	case 32:
		*Value = acpi_md_OsIn32(Address);
		break;

	default:
		return AE_BAD_PARAMETER;
	}

	return AE_OK;
}

/*
 * AcpiOsWritePort:
 *
 *	Write a value to an output port.
 */
ACPI_STATUS
AcpiOsWritePort(ACPI_IO_ADDRESS Address, UINT32 Value, UINT32 Width)
{

	switch (Width) {
	case 8:
		acpi_md_OsOut8(Address, Value);
		break;

	case 16:
		acpi_md_OsOut16(Address, Value);
		break;

	case 32:
		acpi_md_OsOut32(Address, Value);
		break;

	default:
		return AE_BAD_PARAMETER;
	}

	return AE_OK;
}

/*
 * AcpiOsReadMemory:
 *
 *	Read a value from a memory location.
 */
ACPI_STATUS
AcpiOsReadMemory(ACPI_PHYSICAL_ADDRESS Address, UINT32 *Value, UINT32 Width)
{
	void *LogicalAddress;
	ACPI_STATUS rv;

	rv = AcpiOsMapMemory(Address, Width / 8, &LogicalAddress);
	if (rv != AE_OK)
		return rv;

	switch (Width) {
	case 8:
		*Value = *(__volatile uint8_t *) LogicalAddress;
		break;

	case 16:
		*Value = *(__volatile uint16_t *) LogicalAddress;
		break;

	case 32:
		*Value = *(__volatile uint32_t *) LogicalAddress;
		break;

	default:
		rv = AE_BAD_PARAMETER;
	}

	AcpiOsUnmapMemory(LogicalAddress, Width / 8);

	return rv;
}

/*
 * AcpiOsWriteMemory:
 *
 *	Write a value to a memory location.
 */
ACPI_STATUS
AcpiOsWriteMemory(ACPI_PHYSICAL_ADDRESS Address, UINT32 Value, UINT32 Width)
{
	void *LogicalAddress;
	ACPI_STATUS rv;

	rv = AcpiOsMapMemory(Address, Width / 8, &LogicalAddress);
	if (rv != AE_OK)
		return rv;

	switch (Width) {
	case 8:
		*(__volatile uint8_t *) LogicalAddress = Value;
		break;

	case 16:
		*(__volatile uint16_t *) LogicalAddress = Value;
		break;

	case 32:
		*(__volatile uint32_t *) LogicalAddress = Value;
		break;

	default:
		rv = AE_BAD_PARAMETER;
	}

	AcpiOsUnmapMemory(LogicalAddress, Width / 8);

	return rv;
}

/*
 * AcpiOsReadPciConfiguration:
 *
 *	Read a value from a PCI configuration register.
 */
ACPI_STATUS
AcpiOsReadPciConfiguration(ACPI_PCI_ID *PciId, UINT32 Register, void *Value,
    UINT32 Width)
{
	pcitag_t tag;
	pcireg_t tmp;

	/* XXX Need to deal with "segment" ("hose" in Alpha terminology). */

	tag = pci_make_tag(acpi_softc->sc_pc, PciId->Bus, PciId->Device,
	    PciId->Function);
	tmp = pci_conf_read(acpi_softc->sc_pc, tag, Register & ~3);

	switch (Width) {
	case 8:
		*(uint8_t *) Value = (tmp >> ((Register & 3) * 8)) & 0xff;
		break;

	case 16:
		*(uint16_t *) Value = (tmp >> ((Register & 3) * 8)) & 0xffff;
		break;

	case 32:
		*(uint32_t *) Value = tmp;
		break;

	default:
		return AE_BAD_PARAMETER;
	}

	return AE_OK;
}

/*
 * AcpiOsWritePciConfiguration:
 *
 *	Write a value to a PCI configuration register.
 */
ACPI_STATUS
AcpiOsWritePciConfiguration(ACPI_PCI_ID *PciId, UINT32 Register,
    ACPI_INTEGER Value, UINT32 Width)
{
	pcitag_t tag;
	pcireg_t tmp;

	/* XXX Need to deal with "segment" ("hose" in Alpha terminology). */

	tag = pci_make_tag(acpi_softc->sc_pc, PciId->Bus, PciId->Device,
	    PciId->Function);

	switch (Width) {
	case 8:
		tmp = pci_conf_read(acpi_softc->sc_pc, tag, Register & ~3);
		tmp &= ~(0xff << ((Register & 3) * 8));
		tmp |= (Value << ((Register & 3) * 8));
		break;

	case 16:
		tmp = pci_conf_read(acpi_softc->sc_pc, tag, Register & ~3);
		tmp &= ~(0xffff << ((Register & 3) * 8));
		tmp |= (Value << ((Register & 3) * 8));
		break;

	case 32:
		tmp = Value;
		break;

	default:
		return AE_BAD_PARAMETER;
	}

	pci_conf_write(acpi_softc->sc_pc, tag, Register & ~3, tmp);

	return AE_OK;
}

/* get PCI bus# from root bridge recursively */
static int
get_bus_number(
    ACPI_HANDLE        rhandle,
    ACPI_HANDLE        chandle,
    ACPI_PCI_ID        **PciId)
{
	ACPI_HANDLE handle;
	ACPI_STATUS rv;
	ACPI_OBJECT_TYPE type;
	ACPI_PCI_ID *id;
	ACPI_INTEGER v;
	int bus;

	id = *PciId;

	rv = AcpiGetParent(chandle, &handle);
	if (ACPI_FAILURE(rv))
		return 0;

	/*
	 * When handle == rhandle, we have valid PciId->Bus
	 * which was obtained from _BBN in evrgnini.c
	 * so we don't have to reevaluate _BBN.
	 */
	if (handle != rhandle) {
		bus = get_bus_number(rhandle, handle, PciId);

		rv = AcpiGetType(handle, &type);
		if (ACPI_FAILURE(rv) || type != ACPI_TYPE_DEVICE)
			return bus;

		rv = acpi_eval_integer(handle, METHOD_NAME__ADR, &v);

		if (ACPI_FAILURE(rv))
			return bus;

		id->Bus = bus;
		id->Device = ACPI_HIWORD((ACPI_INTEGER)v);
		id->Function = ACPI_LOWORD((ACPI_INTEGER)v);

		/* read HDR_TYPE register */
		rv = AcpiOsReadPciConfiguration(id, 0x0e, &v, 8);
		if (ACPI_SUCCESS(rv) &&
			/* mask multifunction bit & check bridge type */
			((v & 0x7f) == 1 || (v & 0x7f) == 2)) {
			/* read SECONDARY_BUS register */
			rv = AcpiOsReadPciConfiguration(id, 0x19, &v, 8);
			if (ACPI_SUCCESS(rv))
				id->Bus = v;
		}
	}

	return id->Bus;
}

/*
 * AcpiOsDerivePciId:
 *
 * Derive correct PCI bus# by traversing bridges
 */
void
AcpiOsDerivePciId(
    ACPI_HANDLE        rhandle,
    ACPI_HANDLE        chandle,
    ACPI_PCI_ID        **PciId)
{
	(*PciId)->Bus = get_bus_number(rhandle, chandle, PciId);
}
