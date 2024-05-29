import { json } from "@sveltejs/kit";
import type { RequestHandler } from "./$types";

export const POST: RequestHandler = async ({ request, locals: { supabase } }) => {
  const body = await request.json();
  const { mac } = body;

  if (!mac) {
    return json({ error: 'MAC Address is required' }, { status: 400 });
  }

  const { data, error } = await supabase
    .from('devices')
    .update({ connected: true })
    .eq('mac', mac)
    .is('connected', false)
    .gte('created_at', new Date(Date.now() - 1000 * 60 * 5).toISOString())
    .select();
  
  if (error) {
    console.log(Date.now() - 1000 * 60 * 5)
    return json({ error: error.message }, { status: 500 });
  }

  if (!data.length) {
    return json({ error: 'Device not found' }, { status: 400 });
  }

  return json({ message: 'Device is connected' })
}