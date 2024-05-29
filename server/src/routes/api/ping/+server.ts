import { json } from '@sveltejs/kit';
import type { RequestHandler } from './$types';

export const GET: RequestHandler = async () => {
  return new Response('Pong!', { status: 200 });
}

export const POST: RequestHandler = async ({ request }) => {
  const body = await request.json();
  return json({ message: 'Pong!', body });
}