import { json } from '@sveltejs/kit';
import type { RequestHandler } from '@sveltejs/kit';

import nodemailer from 'nodemailer';

import { PUBLIC_NODEMAILER_USER, PUBLIC_NODEMAILER_PASS } from '$env/static/public';

interface ActionsBody {
  mac: string;
  trigger: number;
  state: number;
}

export const POST: RequestHandler = async ({ request, locals: { supabase } }) => {
  const body = await request.json() as ActionsBody;

  const { mac, trigger, state } = body;

  if (!mac) {
    return json({ error: 'MAC Address is required' }, { status: 400 });
  }

  if (trigger === null || isNaN(trigger)) {
    return json({ error: 'Trigger is required' }, { status: 400 });
  }

  if (state === null || isNaN(state)) {
    return json({ error: 'State is required' }, { status: 400 });
  }

  const doesExist = await supabase
    .from('devices')
    .select(`*, profiles(email)`)
    .eq('mac', mac)
    .is('connected', true)
    .maybeSingle();

  if (doesExist.error) {
    return json({ error: doesExist.error.message }, { status: 500 });
  }

  if (!doesExist.data) {
    return json({ error: 'Device not found' }, { status: 404 });
  }

  const isSmoke = trigger === 0;
  const isOn = state;

  const { user, profiles: { email } } = doesExist.data;

  // Delete all actions with the same MAC Address
  await supabase.from('actions').delete().eq('mac', mac);

  const update = await supabase
    .from('devices')
    .update({ status: isOn, smoke: isSmoke })
    .eq('mac', mac)
    .eq('connected', true)
    .eq('user', user);

  const transport = nodemailer.createTransport({
    host: 'smtp.gmail.com',
    port: 465,
    secure: true,
    auth: {
      user: PUBLIC_NODEMAILER_USER,
      pass: PUBLIC_NODEMAILER_PASS
    },
  })

  const info = await transport.sendMail({
    to: email,
    subject: "FireWall Alert",
    html: `
      <h1>FireWall Alert</h1>
      <p>Device: ${doesExist.data.name}</p>
      <p>MAC Address: ${mac}</p>
      <p>Trigger: ${isSmoke ? 'Smoke' : 'Manual'}</p>
      <p>State: ${state}</p>
    `,
  });

  console.log("Message sent: %s", info.messageId);
  
  return json({ message: 'Action completed' });
}