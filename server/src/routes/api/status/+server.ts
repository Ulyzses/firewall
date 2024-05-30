import { text } from '@sveltejs/kit';
import type { RequestHandler } from '@sveltejs/kit';

export const GET: RequestHandler = async ({ url, locals: { supabase } }) => {
  const macAddress = url.searchParams.get('m') as string;
  const state = Number(url.searchParams.get('s')) as number;
  const first = Number(url.searchParams.get('f')) as number;

  if (!macAddress) {
    return text('MAC Address is required', { status: 400 });
  }

  if (!/^([0-9a-fA-F]{2}\:){5}[0-9a-fA-F]{2}$/.test(macAddress)) {
    return text('Invalid MAC Address', { status: 400 });
  }

  // Check if device exists in the server
  const doesExist = await supabase
    .from('devices')
    .select()
    .eq('mac', macAddress)
    .is('connected', true)
    .maybeSingle();
  
  if (doesExist.error) {
    return text(`Error: ${doesExist.error.message}`, { status: 500 });
  }

  if (!doesExist.data) {
    return text('Device not found', { status: 404 });
  }

  // If first request, return last status of the device
  if (first === 1) {
    return text(`${doesExist.data.status ? 'ON' : 'OFF'}`);
  }

  // Get the latest action (if any) for the device within the last 5 minutes
  const queuedAction = await supabase
    .from('actions')
    .select('action')
    .eq('mac', macAddress)
    .gte('created_at', new Date(Date.now() - 1000 * 60 * 5).toISOString())
    .order('created_at', { ascending: false })
    .limit(1)
    .maybeSingle();

  if (queuedAction.error) {
    return text(`Error: ${queuedAction.error.message}`, { status: 500 });
  }

  // Delete all requests for the given MAC address
  await supabase.from('actions').delete().eq('mac', macAddress);

  // Return action if any
  if (queuedAction.data) {
    const updateDevice = await supabase
      .from('devices')
      .update({ status: queuedAction.data.action === "ON" ? 1 : 0 })
      .eq('mac', macAddress)
      .eq('connected', true);

    // If the device is already in the desired state, return 204 No Content
    if ((queuedAction.data.action === "ON" && state === 1) ||
        (queuedAction.data.action === "OFF" && state === 0)) {
      return new Response(null, { status: 204 });
    }
    
    if (updateDevice.error) {
      return text(`Error: ${updateDevice.error.message}`, { status: 500 });
    }

    const updateDevice = await supabase
      .from('devices')
      .update({ status: queuedAction.data.action === "ON" ? 1 : 0 })
      .eq('mac', macAddress)
      .eq('connected', true);
    
    if (updateDevice.error) {
      return text(`Error: ${updateDevice.error.message}`, { status: 500 });
    }

    return text(`${queuedAction.data.action}`);
  } else {
    return new Response(null, { status: 204 });
  }
}